#include <stdio.h>
#include "ft_ping.h"

int main(int argc, char** argv) {
	options_t opt;
	char*	  host_arg = NULL;
	opt = parse_args(argc, argv, &host_arg);
	if (opt.valid != TRUE) {
		parsing_exit(argv[0], &opt);
	}
	printf("valid is %d\n", opt.valid);

	return (0);
}
