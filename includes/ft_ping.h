#ifndef FT_PING_H
#define FT_PING_H

#include <netdb.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0
#define MISSING 2
#define PARSING_ERROR 64
#define INVALID_HOST_ERROR 1

#define ICMP_HEADER_SIZE 8
#define ICMP_PAYLOAD_SIZE 56
#define ICMP_PACKET_SIZE ICMP_PAYLOAD_SIZE + ICMP_HEADER_SIZE

typedef struct {
	uint8_t verbose;
	uint8_t help;
	uint8_t valid;
	char	bad_option;
} options_t;

typedef struct {
	uint16_t pid;
	uint16_t seq_nb;
} icmp_info_t;

options_t		 parse_args(int argc, char** argv, char** host_param);
void			 parsing_exit(char* path, options_t* opt);
void			 print_help();
int				 ft_ping(int verbose, const char* const hostname);
struct addrinfo* get_host_info(const char* const hostname);
void			 error_exit(const char* const error_message);
void			 get_icmp_packet(struct icmp* icmp, icmp_info_t* icmp_info);
#endif
