#include "sys/socket.h"
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "ft_ping.h"

static void		update_packet(packet_t* packet, uint8_t* buff);
static double	get_time(struct icmp* icmp);
static double	handle_echo_reply(packet_t* packet, icmp_info_t* icmp_info);
static void		handle_other_icmp(packet_t* packet, icmp_info_t* icmp_info);
static void		handle_ttl_exceed_icmp(packet_t* packet, icmp_info_t* icmp_info);
static void		print_verbose(uint8_t* payload);
static void		print_raw_ip(uint16_t* payload);
static void		print_ip_header(ip_header_t* header);
static void		print_icmp(struct icmp* icmp, uint16_t size);
static uint8_t	get_flags(uint16_t flags_off);
static uint16_t get_off(uint16_t flags_off);

socket_t init_socket() {
	socket_t sock;
	if ((sock.fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		error(EXIT_FAILURE, errno, "socket failed");
	}

	ssize_t buffsize = SOCKET_BUFFER_SIZE;
	if (setsockopt(sock.fd, SOL_SOCKET, SO_RCVBUF, &buffsize, sizeof(buffsize)) == -1) {
		close(sock.fd);
		error(0, errno, "setsockopt");
	}
	int ttl = FT_TTL_DEFAULT;
	if (setsockopt(sock.fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1) {
		close(sock.fd);
		error(0, errno, "setsockpt");
	}
	return (sock);
}

double read_socket(socket_t sock, icmp_info_t* icmp_info) {
	uint8_t	 buff[SOCKET_RECEIVE_BUFFER_SIZE];
	double	 time = NAN;
	packet_t packet;
	packet.len = recv(sock.fd, buff, SOCKET_RECEIVE_BUFFER_SIZE, 0);

	if (packet.len > 0) {
		packet.len -= IP_HEADER_SIZE;
		update_packet(&packet, buff);

		if (packet.icmp->icmp_type == 0) {
			time = handle_echo_reply(&packet, icmp_info);

		} else {
			handle_other_icmp(&packet, icmp_info);
		}
	}
	return (time);
}

static void update_packet(packet_t* packet, uint8_t* buff) {
	packet->raw = ((uint8_t*)buff + IP_HEADER_SIZE + ICMP_TIME_EXCEED_HEADER_SIZE);
	packet->ttl = ((ip_header_t*)buff)->ttl;
	packet->source = ((ip_header_t*)buff)->srcip;
	packet->icmp = (struct icmp*)((uint8_t*)buff + IP_HEADER_SIZE);
}

static double get_time(struct icmp* icmp) {
	struct timeval* sent_time = (struct timeval*)icmp->icmp_data;
	struct timeval	recv_time;
	gettimeofday(&recv_time, NULL);
	double time = (double)(recv_time.tv_sec - sent_time->tv_sec) * 1000. +
				  (double)(recv_time.tv_usec - sent_time->tv_usec) / 1000.;
	return (time);
}

static double handle_echo_reply(packet_t* packet, icmp_info_t* icmp_info) {
	double time = NAN;
	if (ntohs(packet->icmp->icmp_id) == icmp_info->pid) {
		if (check_checksum(packet) == 0) {
			time = get_time(packet->icmp);
			printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", packet->len,
				   inet_ntoa(*(struct in_addr*)&packet->source), ntohs(packet->icmp->icmp_seq),
				   packet->ttl, time);
		} else {
			fprintf(stderr, "checksum mismatch from %s\n", icmp_info->hostname);
		}
	}
	return (time);
}

static void handle_other_icmp(packet_t* packet, icmp_info_t* icmp_info) {
	struct icmp* origin_icmp = (struct icmp*)((uint8_t*)(packet->raw) + IP_HEADER_SIZE);

	if (ntohs(origin_icmp->icmp_id) == icmp_info->pid) {
		if (packet->icmp->icmp_type == ICMP_TIME_EXCEEDED) {
			handle_ttl_exceed_icmp(packet, icmp_info);
		} else {
			fprintf(stderr, "Bad ICMP type: %d\n", packet->icmp->icmp_type);
		}
	}
}

static void handle_ttl_exceed_icmp(packet_t* packet, icmp_info_t* icmp_info) {
	if (check_checksum(packet) == 0) {
		printf("%ld bytes from %s: Time to live exceeded\n", packet->len,
			   inet_ntoa(*(struct in_addr*)&packet->source));
		if (icmp_info->verbose) {
			print_verbose(packet->raw);
		}
	} else {
		fprintf(stderr, "checksum mismatch from %s\n",
				inet_ntoa(*(struct in_addr*)&packet->source));
	}
}

int write_socket(socket_t sock, struct sockaddr_in* dest_addr, icmp_info_t* icmp_info) {
	char buff[ICMP_PACKET_SIZE];

	get_icmp_packet((struct icmp*)buff, icmp_info);
	if (sendto(sock.fd, buff, ICMP_PACKET_SIZE, 0, (const struct sockaddr*)dest_addr,
			   sizeof(struct sockaddr)) < 0) {
		error(EXIT_FAILURE, errno, "sending packet");
	}
	update_icmp_info(icmp_info);
	return (1);
}

static void print_verbose(uint8_t* payload) {
	ip_header_t* header = (ip_header_t*)payload;
	uint16_t	 size = ntohs(header->length) - IP_HEADER_SIZE;
	struct icmp* icmp = (struct icmp*)((uint8_t*)payload + IP_HEADER_SIZE);

	print_raw_ip((uint16_t*)payload);
	print_ip_header(header);
	print_icmp(icmp, size);
}

static void print_raw_ip(uint16_t* payload) {
	printf("IP Hdr Dump:\n");
	for (int i = 0; i < (IP_HEADER_SIZE / 2); ++i) {
		printf(" %04hx", ntohs(((uint16_t*)payload)[i]));
	}
}

static void print_ip_header(ip_header_t* header) {
	printf("\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data");
	printf("\n%x   %x  %02x %04x %04x   %x %04x  %02x  %02x %04x %s", header->version,
		   header->hdrlen, ntohs(header->dscp), ntohs(header->length), ntohs(header->ident),
		   get_flags(header->flags_off), get_off(header->flags_off), header->ttl, header->protocol,
		   ntohs(header->checksum), inet_ntoa(*(struct in_addr*)&header->srcip));
	printf("  %s\n", inet_ntoa(*(struct in_addr*)&header->dstip));
}

static inline uint8_t get_flags(uint16_t flags_off) {
	uint16_t tmp = ntohs(flags_off) >> 13;
	return ((uint8_t)tmp);
}

static inline uint16_t get_off(uint16_t flags_off) {
	return (ntohs(flags_off) & 0x1FFF);
}

static void print_icmp(struct icmp* icmp, uint16_t size) {
	printf("ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n", icmp->icmp_type,
		   icmp->icmp_code, size, ntohs(icmp->icmp_id), ntohs(icmp->icmp_seq));
}
