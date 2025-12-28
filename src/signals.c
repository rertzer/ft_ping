#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "ft_ping.h"

extern struct sockaddr_in* host_addr;

void init_signals() {
	struct sigaction sa;
	sa.sa_handler = send_icmp;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGALRM, &sa, NULL);
}

void send_icmp(int sig) {
	(void)sig;
	char		 buff[ICMP_PACKET_SIZE];
	struct icmp* icmp = (struct icmp*)buff;

	get_icmp_packet(icmp);
	update_icmp_info();

	alarm(1);
	return;
}
