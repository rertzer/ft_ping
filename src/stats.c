#include <math.h>
#include <stdio.h>

#include "ft_ping.h"

void init_stats(stats_t* stats) {
	*stats = (stats_t){0, 0, 0, 3.0e+38F, 0.F, 0.F, 0.F};
}

void update_stats(stats_t* stats, float time) {
	stats->received++;
	if (time < stats->min) {
		stats->min = time;
	} else if (time > stats->max) {
		stats->max = time;
	}
	stats->avg += time;
	stats->stddev += pow(time, 2.);
}

void compute_stats(stats_t* stats) {
	stats->loss = (float)(stats->transmitted - stats->received) / (float)stats->transmitted * 100.F;
	stats->avg /= (float)stats->received;
	stats->stddev = sqrt((double)((stats->stddev / (float)stats->received) - pow(stats->avg, 2.)));
}

void print_stats(const char* const hostname, stats_t* stats) {
	printf("--- %s ping statistics\n", hostname);
	printf("%d packets transmitted, %d packets received, %.0f%% packet loss\n", stats->transmitted,
		   stats->received, stats->loss);
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", stats->min, stats->avg,
		   stats->max, stats->stddev);
}
