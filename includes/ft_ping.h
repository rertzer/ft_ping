#ifndef FT_PING_H
#define FT_PING_H

#include <netdb.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0
#define MISSING 2
#define PARSING_ERROR 64
#define INVALID_HOST_ERROR 1

#define IP_HEADER_SIZE 20
#define ICMP_TIME_EXCEED_HEADER_SIZE 8
#define ICMP_HEADER_SIZE 8
#define ICMP_PAYLOAD_SIZE 56
#define ICMP_PACKET_SIZE (ICMP_PAYLOAD_SIZE + ICMP_HEADER_SIZE)

#define SELECT_MAX_FD 4
#define SOCKET_BUFFER_SIZE 65536
#define SOCKET_RECEIVE_BUFFER_SIZE 600
#define FT_TTL_DEFAULT 64

typedef struct {
	uint8_t verbose;
	uint8_t help;
	uint8_t valid;
	char	bad_option;
} options_t;

typedef struct {
	char const* hostname;
	uint16_t	pid;
	uint16_t	seq_nb;
	uint8_t		verbose;
} icmp_info_t;

typedef struct {
	int				   fd;
	socklen_t		   len;
	struct sockaddr_in addr;
} socket_t;

typedef struct {
	uint8_t*	 raw;
	ssize_t		 len;
	uint8_t		 ttl;
	uint32_t	 source;
	struct icmp* icmp;
} packet_t;

typedef struct {
	int	   transmitted;
	int	   received;
	double loss;
	double min;
	double max;
	double avg;
	double stddev;
} stats_t;

typedef struct {
	uint8_t	 hdrlen : 4;
	uint8_t	 version : 4;
	uint8_t	 ecn : 2;
	uint8_t	 dscp : 6;
	uint16_t length;
	uint16_t ident;
	uint16_t flags_off;
	uint8_t	 ttl;
	uint8_t	 protocol;
	uint16_t checksum;
	uint32_t srcip;
	uint32_t dstip;
	uint32_t options[];	 // Present if hdrlen > 5
} __attribute__((__packed__)) ip_header_t;

options_t			parse_args(int argc, char** argv, char** host_param);
void				parsing_exit(char* path, options_t* opt);
bool				valid_parsing(options_t* opt);
void				print_help();
int					ft_ping(int verbose, const char* const hostname);
struct sockaddr_in* init_host(const char* const hostname);
void				get_icmp_packet(struct icmp* icmp, icmp_info_t* icmp_info);
icmp_info_t			init_icmp_info(const char* hostname, uint8_t verbose);
void				update_icmp_info(icmp_info_t* icmp_info);
void				init_signals();
sigset_t			init_sigmask();
void				send_icmp(icmp_info_t* icmp_info);
uint16_t			check_checksum(packet_t* packet);
void				set_send_next(int sig);
void				set_the_end(int sig);
socket_t			init_socket();
fd_set				init_fd_set(int fd);
double				read_socket(socket_t sock, icmp_info_t* icmp_info);
stats_t				init_stats();
void				update_stats(stats_t* stats, double time);
void				compute_stats(stats_t* stats);
void				print_stats(const char* const hostname, stats_t* stats);
int write_socket(socket_t sock, struct sockaddr_in* dest_addr, icmp_info_t* icmp_info);
#endif
