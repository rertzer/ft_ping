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

#define ICMP_HEADER_SIZE 8
#define ICMP_PAYLOAD_SIZE 56
#define ICMP_PACKET_SIZE (ICMP_PAYLOAD_SIZE + ICMP_HEADER_SIZE)

#define SELECT_MAX_FD 4

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
} icmp_info_t;

typedef struct {
	int				   fd;
	socklen_t		   len;
	struct sockaddr_in addr;
} socket_t;

typedef struct {
	ssize_t		 len;
	uint8_t		 ttl;
	uint8_t*	 source;
	struct icmp* icmp;
} packet_t;

typedef struct {
	int	  transmitted;
	int	  received;
	float loss;
	float min;
	float max;
	float avg;
	float stddev;
} stats_t;

options_t			parse_args(int argc, char** argv, char** host_param);
void				parsing_exit(char* path, options_t* opt);
bool				valid_parsing(options_t* opt);
void				print_help();
int					ft_ping(int verbose, const char* const hostname);
struct sockaddr_in* init_host(const char* const hostname);
void				error_exit(const char* const error_message);
void				get_icmp_packet(struct icmp* icmp, icmp_info_t* icmp_info);
icmp_info_t			init_icmp_info(const char* hostname);
void				update_icmp_info(icmp_info_t* icmp_info);
void				init_signals();
sigset_t			init_sigmask();
void				send_icmp(icmp_info_t* icmp_info);
uint16_t			check_checksum(struct icmp* icmp);
void				set_send_next(int sig);
void				set_the_end(int sig);
socket_t			init_socket();
fd_set				init_fd_set(int fd);
float				read_socket(socket_t sock, icmp_info_t* icmp_info);
void				init_stats(stats_t* stats);
void				update_stats(stats_t* stats, float time);
void				compute_stats(stats_t* stats);
void				print_stats(const char* const hostname, stats_t* stats);
int write_socket(socket_t sock, struct sockaddr_in* dest_addr, icmp_info_t* icmp_info);
#endif
