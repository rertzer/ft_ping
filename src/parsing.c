#include "ft_ping.h"

static int		   parse_one_arg(char* arg, options_t* opt, char** host_param);
static int		   parse_option(char* arg, options_t* opt);
static inline void parse_host(char* arg, char** host_param);

options_t parse_args(int argc, char** argv, char** host_param) {
	options_t opt = {FALSE, FALSE, TRUE, 0};

	for (int i = 1; i < argc; ++i) {
		if ((parse_one_arg(argv[i], &opt, host_param) != 0) || opt.help == TRUE) {
			return (opt);
		}
	}
	if (*host_param == NULL) {
		opt.valid = MISSING;
	}
	return (opt);
}

static int parse_one_arg(char* arg, options_t* opt, char** host_param) {
	int error = 0;

	if (arg[0] == '-') {
		error = parse_option(arg, opt);
	} else if (*host_param == NULL) {
		parse_host(arg, host_param);
	}
	return (error);
}

static int parse_option(char* arg, options_t* opt) {
	int error = 0;

	for (int i = 1; arg[i] != 0; ++i) {
		switch (arg[i]) {
			case 'v':
				opt->verbose = TRUE;
				break;
			case '?':
				opt->help = TRUE;
				break;
			default:
				opt->valid = FALSE;
				opt->bad_option = arg[i];
				error = 1;
				break;
		}
	}
	return (error);
}

static inline void parse_host(char* arg, char** host_param) {
	*host_param = arg;
}

bool valid_parsing(options_t* opt) {
	return (!(opt->valid == FALSE || (opt->valid == MISSING && opt->help == FALSE)));
}
