#include <stdio.h>
#include "ft_ping.h"

int main(int argc, char** argv) {
	options_t opt;
	char*	  host_arg = NULL;
	opt = parse_args(argc, argv, &host_arg);
	if (opt.valid == FALSE || (opt.valid == MISSING && opt.help == FALSE)) {
		parsing_exit(argv[0], &opt);
	}
	if (opt.help == TRUE) {
		print_help();
		return (0);
	}

	return (0);
}
