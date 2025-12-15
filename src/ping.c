#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ft_ping.h"

extern bool send_next;
extern bool the_end;

int ft_ping(int verbose, const char* const hostname) {
	(void)verbose;

	struct sockaddr_in* host_addr = init_host(hostname);
	icmp_info_t			icmp_info = init_icmp_info();
	socket_t			sock = init_socket();
	fd_set				active = init_fd_set(sock.fd);
	sigset_t			sigmask = init_sigmask();
	printf("sock_fd is %d\n", sock.fd);
	init_signals();
	printf("ft ping\n");
	while (the_end == false) {
		printf("while loop\n");
		fd_set* readfd = &active;
		// printf("start pselect, writefd ready ? %d\n", ok);
		int fd_nb = pselect(SELECT_MAX_FD, readfd, NULL, NULL, NULL, &sigmask);
		printf("inside pselect, received %d\n", fd_nb);
		printf("error %d: %s\n", errno, strerror(errno));
		if (fd_nb > 0) {
			printf("read_socket\n");
			if (FD_ISSET(sock.fd, readfd)) {
				printf("read_socket\n");
				read_socket(sock);
			}
		} else if (send_next == true) {
			printf("write_socket\n");
			write_socket(sock, host_addr, &icmp_info);
			send_next = false;
			printf("send_next OFF\n");
		}
	}
	return (0);
}

fd_set init_fd_set(int fd) {
	fd_set active;

	FD_ZERO(&active);
	FD_SET(fd, &active);

	return (active);
}
