#include <stdio.h>
#include "ft_ping.h"

void print_help() {
	char* help_string =
		"Usage: ft_ping [OPTION...] HOST ...\n"
		"Send ICMP ECHO_REQUEST packets to network hosts.\n\n"
		" Options valid for all request types:\n"
		" -v                          verbose output\n"
		" -?                          give this help list\n\n"
		"Mandatory or optional arguments to long options are also mandatory or optional\n"
		"for any corresponding short options.\n\n"
		"Options marked with (root only) are available only to superuser.\n\n"
		"Report bugs to <rertzer@student.42.fr>.";

	printf("%s", help_string);
}
