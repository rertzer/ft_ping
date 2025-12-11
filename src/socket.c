#include "sys/socket.h"
#include "ft_ping.h"

socket_t init_socket() {
	socket_t sock;
	sock.fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	ssize_t buffsize = 64 * 1024;
	setsockopt(sock.fd, SOL_SOCKET, SO_RCVBUF, &buffsize, sizeof(buffsize));
	return (sock);
}

void read_socket(socket_t sock) {}

void write_socket(socket_t sock, struct sockaddr_in* dest_addr, icmp_info_t icmp_info) {}
