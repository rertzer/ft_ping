#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ft_ping.h"

extern bool send_next;
extern bool the_end;

static void print_ping(struct sockaddr_in* host_addr, icmp_info_t* icmp_info, bool verbose);

int ft_ping(int verbose, const char* const hostname) {
	struct sockaddr_in* host_addr = init_host(hostname);
	icmp_info_t			icmp_info = init_icmp_info(hostname);
	socket_t			sock = init_socket();
	fd_set				active = init_fd_set(sock.fd);
	sigset_t			sigmask = init_sigmask();
	stats_t				stats = init_stats();

	init_signals();

	print_ping(host_addr, &icmp_info, verbose);

	while (the_end == false) {
		fd_set* readfd = &active;
		int		fd_nb = pselect(SELECT_MAX_FD, readfd, NULL, NULL, NULL, &sigmask);

		if (fd_nb > 0) {
			if (FD_ISSET(sock.fd, readfd)) {
				double time = read_socket(sock, &icmp_info);
				if (time == time) {
					update_stats(&stats, time);
				}
			}
		} else {
			if (errno == EINTR) {
				if (send_next == true) {
					int sent = write_socket(sock, host_addr, &icmp_info);
					if (sent) {
						stats.transmitted += sent;
					}
					send_next = false;
				}
			} else {
				close(sock.fd);
				error(EXIT_FAILURE, errno, "select failed");
			}
		}
	}

	close(sock.fd);
	fflush(stdout);
	compute_stats(&stats);
	print_stats(hostname, &stats);

	return (0);
}

static void print_ping(struct sockaddr_in* host_addr, icmp_info_t* icmp_info, bool verbose) {
	printf("PING %s (%s): 56 data bytes", icmp_info->hostname, inet_ntoa(host_addr->sin_addr));
	if (verbose) {
		printf(", id 0x%X = %d", icmp_info->pid, icmp_info->pid);
	}
	printf("\n");
}

fd_set init_fd_set(int fd) {
	fd_set active;

	FD_ZERO(&active);
	FD_SET(fd, &active);

	return (active);
}
