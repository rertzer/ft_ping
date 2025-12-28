#include <stdio.h>
#include <unistd.h>
#include "ft_ping.h"

int ft_ping(int verbose, const char* const hostname) {
	(void)verbose;
	init_host(hostname);
	init_icmp_info();
	init_signals();
	init_socket();
	socket_t sock;
	sock.fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	ssize_t buffsize = 60 * 1024;
	setsockopt(sock.fd, SOL_SOCKET, SO_RCVBUF, &buffsize, sizeof(buffsize));
	if (bind(sock.fd, (const struct sockaddr*)&sock.addr, sizeof(sock.addr)) != 0) {
		printf("socket bind error\n");
		return (1);
	}

	alarm(1);
	while (true) {
		printf("looping\n");
		pause();
		printf("woke up\n");
	}
	return (0);
}
