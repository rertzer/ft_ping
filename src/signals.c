#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "ft_ping.h"

bool					   send_next;
bool					   the_end;
extern struct sockaddr_in* host_addr;

void init_signals() {
	send_next = false;
	the_end = false;
	struct sigaction sa;
	sa.sa_handler = set_send_next;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGALRM, &sa, NULL);
	alarm(1);
}

void set_send_next(int sig) {
	(void)sig;
	send_next = true;
	alarm(1);
}

void send_icmp(icmp_info_t* icmp_info) {
	char		 buff[ICMP_PACKET_SIZE];
	struct icmp* icmp = (struct icmp*)buff;

	get_icmp_packet(icmp, icmp_info);
	update_icmp_info(icmp_info);
}
sigset_t init_sigmask() {
	sigset_t sigmask;
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGALRM);
	sigaddset(&sigmask, SIGINT);
	return (sigmask);
}
