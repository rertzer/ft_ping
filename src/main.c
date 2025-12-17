#include "ft_ping.h"

int main(int argc, char** argv) {
	options_t opt;
	char*	  host_arg = NULL;
	opt = parse_args(argc, argv, &host_arg);
	if (!valid_parsing(&opt)) {
		parsing_exit(argv[0], &opt);
	}
	if (opt.help == TRUE) {
		print_help();
		return (0);
	}
	int ret_value = ft_ping(opt.verbose, host_arg);

	return (ret_value);
}
