#include "sys/socket.h"
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "ft_ping.h"

static void	  update_packet(packet_t* packet, uint8_t* buff);
static double get_time(struct icmp* icmp);
static double handle_echo_reply(packet_t* packet, icmp_info_t* icmp_info);
static void	  handle_other_icmp(packet_t* packet, uint8_t* buff, int pid);

socket_t init_socket() {
	socket_t sock;
	if ((sock.fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		error(EXIT_FAILURE, errno, "socket failed");
	}

	ssize_t buffsize = 65536;
	if (setsockopt(sock.fd, SOL_SOCKET, SO_RCVBUF, &buffsize, sizeof(buffsize)) == -1) {
		close(sock.fd);
		error(0, errno, "setsockopt");
	}
	int ttl = 255;
	if (setsockopt(sock.fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1) {
		close(sock.fd);
		error(0, errno, "setsockpt");
	}
	return (sock);
}

double read_socket(socket_t sock, icmp_info_t* icmp_info) {
	uint8_t	 buff[500];
	packet_t packet;
	packet.len = recv(sock.fd, buff, 500, 0);
	double time = NAN;

	if (packet.len > 0) {
		packet.len -= 20;
		update_packet(&packet, buff);

		if (packet.icmp->icmp_type == 0) {
			time = handle_echo_reply(&packet, icmp_info);

		} else {
			handle_other_icmp(&packet, buff, icmp_info->pid);
		}
	}
	return (time);
}

static void update_packet(packet_t* packet, uint8_t* buff) {
	packet->ttl = *((uint8_t*)buff + 8);
	packet->source = ((uint8_t*)buff + 12);
	packet->icmp = (struct icmp*)((uint8_t*)buff + 20);
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
		if (check_checksum(packet->icmp) == 0) {
			time = get_time(packet->icmp);
			printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", packet->len,
				   inet_ntoa(*(struct in_addr*)packet->source), ntohs(packet->icmp->icmp_seq),
				   packet->ttl, time);
		} else {
			printf("checksum mismatch from %s\n", icmp_info->hostname);
		}
	}
	return (time);
}

static void handle_other_icmp(packet_t* packet, uint8_t* buff, int pid) {
	struct icmp* origin_icmp = (struct icmp*)((uint8_t*)buff + 48);
	if (ntohs(origin_icmp->icmp_id) == pid) {
		if (packet->icmp->icmp_type == 11) {
			if (check_checksum(packet->icmp) == 0) {
				printf("%ld bytes from %s: Time to live exceeded\n", packet->len,
					   inet_ntoa(*(struct in_addr*)packet->source));
			} else {
				printf("checksum mismatch from %s\n", inet_ntoa(*(struct in_addr*)packet->source));
			}
		} else {
			printf("Bad ICMP type: %d\n", packet->icmp->icmp_type);
		}
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
