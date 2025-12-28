#include <stdio.h>
#include "ft_ping.h"

void parsing_exit(char* path, options_t* opt) {
	switch (opt->valid) {
		case FALSE:
			fprintf(stderr, "%s: invalid option -- '%c'\n", path, opt->bad_option);
			break;
		case MISSING:
			fprintf(stderr, "ft_ping: missing host operand\n");
			break;
		default:
			break;
	}
	fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
	exit(PARSING_ERROR);
}
