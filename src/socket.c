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

static float get_time(struct icmp* icmp);

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
	char	buff[500];
	ssize_t len = recv(sock.fd, buff, 500, 0);
	float	time = NAN;
	if (len > 0) {
		uint8_t		 ttl = *((uint8_t*)buff + 8);
		uint8_t*	 source_p = ((uint8_t*)buff + 12);
		struct icmp* icmp = (struct icmp*)((uint8_t*)buff + 20);
		int			 recv_id = ntohs(icmp->icmp_id);
		if (recv_id == pid) {
			time = get_time((struct icmp*)icmp);
			printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", len - 20,
				   inet_ntoa(*(struct in_addr*)source_p), ntohs(icmp->icmp_seq), ttl, time);
		}
	} else {
		printf("read error %ld\n", len);
	}
	return (time);
}

static float get_time(struct icmp* icmp) {
	struct timeval* sent_time = (struct timeval*)icmp->icmp_data;
	struct timeval	recv_time;
	gettimeofday(&recv_time, NULL);
	float time = (float)(recv_time.tv_sec - sent_time->tv_sec) * 1000. +
				 (float)(recv_time.tv_usec - sent_time->tv_usec) / 1000.;
	return (time);
}

int write_socket(socket_t sock, struct sockaddr_in* dest_addr, icmp_info_t* icmp_info) {
	char buff[ICMP_PACKET_SIZE];

	get_icmp_packet((struct icmp*)buff, icmp_info);
	int len = sendto(sock.fd, buff, ICMP_PACKET_SIZE, 0, (const struct sockaddr*)dest_addr,
					 sizeof(struct sockaddr));
	if (len < 0) {
		printf("error: %d %s\n", errno, strerror(errno));
		return (0);
	}
	update_icmp_info(icmp_info);
	return (1);
}
