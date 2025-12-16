#include <ft_ping.h>
#include <math.h>

void init_stats(stats_t* stats) {
	*stats = (stats_t){0, 0, 0, 0.0, 3.0e+38F, 0.0, 0.0};
}

void update_stats(stats_t* stats, float time) {
	stats->received++;
	if (time < stats->min) {
		stats->min = time;
	} else if (time > stats->max) {
		stats->max = time;
	}
	stats->avg += time;
	stats->stddev += pow(time, 2);
}
