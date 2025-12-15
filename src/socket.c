#include "sys/socket.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "ft_ping.h"

socket_t init_socket() {
	socket_t sock;
	sock.fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	ssize_t buffsize = 64 * 1024;
	setsockopt(sock.fd, SOL_SOCKET, SO_RCVBUF, &buffsize, sizeof(buffsize));
	return (sock);
}

void read_socket(socket_t sock) {
	char	buff[500];
	ssize_t len = recv(sock.fd, buff, 500, 0);
	if (len > 0) {
		printf("received size %ld sized response\n", len);
		uint8_t* icmp = (uint8_t*)buff + 20;
		print_icmp((struct icmp*)icmp);
	} else {
		printf("read error %ld\n", len);
	}
}

void write_socket(socket_t sock, struct sockaddr_in* dest_addr, icmp_info_t* icmp_info) {
	char buff[ICMP_PACKET_SIZE];

	get_icmp_packet((struct icmp*)buff, icmp_info);
	update_icmp_info(icmp_info);
	int len = sendto(sock.fd, buff, ICMP_PACKET_SIZE, 0, (const struct sockaddr*)dest_addr,
					 sizeof(struct sockaddr));
	printf("sendto ret: %d\n", len);
	if (len < 0) {
		printf("error: %d %s\n", errno, strerror(errno));
	}
}
