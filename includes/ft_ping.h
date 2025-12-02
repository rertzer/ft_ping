#ifndef FT_PING_H
#define FT_PING_H

#include <stdint.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MISSING 2
#define PARSING_ERROR 64
#define INVALID_HOST_ERROR 1

typedef struct {
	uint8_t verbose;
	uint8_t help;
	uint8_t valid;
	char	bad_option;
} options_t;

options_t parse_args(int argc, char** argv, char** host_param);
void	  parsing_exit(char* path, options_t* opt);
#endif
