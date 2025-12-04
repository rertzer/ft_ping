#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>

#include "ft_ping.h"

int ft_ping(int verbose, const char* const hostname) {
	(void)verbose;
	struct addrinfo*	host = get_host_info(hostname);
	struct sockaddr_in* host_addr = (struct sockaddr_in*)host->ai_addr;
	printf("Host name: %s, port: %u, address: %s\n", host->ai_canonname, host_addr->sin_port,
		   inet_ntoa(host_addr->sin_addr));
	return (0);
}

struct addrinfo* get_host_info(const char* const hostname) {
	struct addrinfo hints, *host;
	bzero(&hints, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;

	int errcode = getaddrinfo(hostname, NULL, &hints, &host);
	if (errcode != 0) {
		error_exit(gai_strerror(errcode));
	}
	return (host);
}
