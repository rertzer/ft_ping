#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
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
	stats_t				stats;
	init_stats(&stats);
	init_signals();
	printf("PING %s (%s): 56 data bytes\n", hostname, inet_ntoa(host_addr->sin_addr));
	while (the_end == false) {
		fd_set* readfd = &active;
		int		fd_nb = pselect(SELECT_MAX_FD, readfd, NULL, NULL, NULL, &sigmask);
		if (fd_nb > 0) {
			if (FD_ISSET(sock.fd, readfd)) {
				float time = read_socket(sock, icmp_info.pid);
				if (time != NAN) {
					update_stats(&stats, time);
				}
			}
		} else if (send_next == true) {
			int sent = write_socket(sock, host_addr, &icmp_info);
			if (sent) {
				stats.transmitted += sent;
			}
			send_next = false;
		}
	}
	compute_stats(&stats);
	print_stats(hostname, &stats);
	return (0);
}

fd_set init_fd_set(int fd) {
	fd_set active;

	FD_ZERO(&active);
	FD_SET(fd, &active);

	return (active);
}
