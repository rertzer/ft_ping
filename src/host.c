#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>

#include "ft_ping.h"

static struct addrinfo* get_host_info(const char* const hostname);

struct sockaddr_in* init_host(const char* const hostname) {
	struct addrinfo*	host = get_host_info(hostname);
	struct sockaddr_in* host_addr = (struct sockaddr_in*)host->ai_addr;
	return (host_addr);
}

static struct addrinfo* get_host_info(const char* const hostname) {
	struct addrinfo hints, *host;
	bzero(&hints, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;

	int errcode = getaddrinfo(hostname, NULL, &hints, &host);
	if (errcode != 0) {
		error_exit(gai_strerror(errcode));
	}
	return (host);
}
