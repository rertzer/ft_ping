#include <signal.h>
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
	signal(SIGINT, set_the_end);
	alarm(1);
}

void set_send_next(int sig) {
	(void)sig;
	send_next = true;
	alarm(1);
}

void set_the_end(int sig) {
	(void)sig;
	the_end = true;
}

sigset_t init_sigmask() {
	sigset_t sigmask;
	sigfillset(&sigmask);
	sigdelset(&sigmask, SIGALRM);
	sigdelset(&sigmask, SIGINT);
	return (sigmask);
}
