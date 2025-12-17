#include "sys/socket.h"
#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "ft_ping.h"

static void	 update_packet(packet_t* packet, uint8_t* buff);
static float get_time(struct icmp* icmp);
static float handle_echo_reply(packet_t* packet, int pid);
static void	 handle_other_icmp(packet_t* packet, uint8_t* buff, int pid);

socket_t init_socket() {
	socket_t sock;
	sock.fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	ssize_t buffsize = 64 * 1024;
	setsockopt(sock.fd, SOL_SOCKET, SO_RCVBUF, &buffsize, sizeof(buffsize));
	int ttl = 255;
	setsockopt(sock.fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
	return (sock);
}

float read_socket(socket_t sock, uint16_t pid) {
	uint8_t	 buff[500];
	packet_t packet;
	packet.len = recv(sock.fd, buff, 500, 0);
	float time = NAN;

	if (packet.len > 0) {
		packet.len -= 20;
		update_packet(&packet, buff);

		if (packet.icmp->icmp_type == 0) {
			time = handle_echo_reply(&packet, pid);

		} else {
			handle_other_icmp(&packet, buff, pid);
		}

	} else {
		printf("ft_ping: read error %ld\n", packet.len);
	}
	return (time);
}

static void update_packet(packet_t* packet, uint8_t* buff) {
	packet->ttl = *((uint8_t*)buff + 8);
	packet->source = ((uint8_t*)buff + 12);
	packet->icmp = (struct icmp*)((uint8_t*)buff + 20);
}

static float get_time(struct icmp* icmp) {
	struct timeval* sent_time = (struct timeval*)icmp->icmp_data;
	struct timeval	recv_time;
	gettimeofday(&recv_time, NULL);
	float time = (float)(recv_time.tv_sec - sent_time->tv_sec) * 1000. +
				 (float)(recv_time.tv_usec - sent_time->tv_usec) / 1000.;
	return (time);
}

static float handle_echo_reply(packet_t* packet, int pid) {
	float time = NAN;
	if (ntohs(packet->icmp->icmp_id) == pid) {
		time = get_time(packet->icmp);
		printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", packet->len,
			   inet_ntoa(*(struct in_addr*)packet->source), ntohs(packet->icmp->icmp_seq),
			   packet->ttl, time);
	}
	return (time);
}

static void handle_other_icmp(packet_t* packet, uint8_t* buff, int pid) {
	struct icmp* origin_icmp = (struct icmp*)((uint8_t*)buff + 48);
	if (ntohs(origin_icmp->icmp_id) == pid) {
		if (packet->icmp->icmp_type == 11) {
			printf("%ld bytes from %s: Time to live exceeded\n", packet->len,
				   inet_ntoa(*(struct in_addr*)packet->source));
		} else {
			printf("Bad ICMP type: %d\n", packet->icmp->icmp_type);
		}
	}
}

int write_socket(socket_t sock, struct sockaddr_in* dest_addr, icmp_info_t* icmp_info) {
	char buff[ICMP_PACKET_SIZE];

	get_icmp_packet((struct icmp*)buff, icmp_info);
	int len = sendto(sock.fd, buff, ICMP_PACKET_SIZE, 0, (const struct sockaddr*)dest_addr,
					 sizeof(struct sockaddr));
	if (len < 0) {
		printf("ft_ping: error: %d %s\n", errno, strerror(errno));
		return (0);
	}
	update_icmp_info(icmp_info);
	return (1);
}
