#include <math.h>
#include <stdio.h>

#include "ft_ping.h"

stats_t init_stats() {
	return ((stats_t){0, 0, 0, 3.0e+38, 0., 0., 0.});
}

void update_stats(stats_t* stats, double time) {
	stats->received++;
	if (time < stats->min) {
		stats->min = time;
	}
	if (time > stats->max) {
		stats->max = time;
	}
	stats->avg += time;
	stats->stddev += pow(time, 2.);
}

void compute_stats(stats_t* stats) {
	if (stats->transmitted > 0) {
		stats->loss =
			(double)(stats->transmitted - stats->received) / (double)stats->transmitted * 100.;
	}
	if (stats->received != 0) {
		stats->avg /= (double)stats->received;
		stats->stddev =
			sqrt((double)((stats->stddev / (double)stats->received) - pow(stats->avg, 2.)));
	}
}

void print_stats(const char* const hostname, stats_t* stats) {
	printf("--- %s ping statistics ---\n", hostname);
	printf("%d packets transmitted, %d packets received, %.0f%% packet loss\n", stats->transmitted,
		   stats->received, stats->loss);
	if (stats->received != 0) {
		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", stats->min, stats->avg,
			   stats->max, stats->stddev);
	}
}
