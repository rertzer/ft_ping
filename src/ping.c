#include <stdio.h>
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

	init_signals();
	while (the_end == false) {
		fd_set* readfd = &active;
		fd_set* writefd = set_writefd(&active);
		int		fd_nb = pselect(SELECT_MAX_FD, readfd, writefd, NULL, NULL, &sigmask);
		if (fd_nb > 0) {
			if (FD_ISSET(sock.fd, readfd)) {
				read_socket(sock);
			} else if (FD_ISSET(sock.fd, writefd)) {
				write_socket(sock);
				send_next = false;
			}
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

fd_set* set_writefd(fd_set* active) {
	fd_set* writefd = NULL;
	if (send_next) {
		writefd = active;
	}

	return (writefd);
}
