#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "ft_ping.h"

static uint16_t get_icmp_checksum(uint16_t* icmp);

void get_icmp_packet(struct icmp* icmp, icmp_info_t* icmp_info) {
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_id = htons(icmp_info->pid);
	icmp->icmp_seq = htons(icmp_info->seq_nb);
	memset(icmp->icmp_data, 0xa5, ICMP_PAYLOAD_SIZE);
	gettimeofday((struct timeval*)icmp->icmp_data, NULL);
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = get_icmp_checksum((uint16_t*)icmp);
}

static uint16_t get_icmp_checksum(uint16_t* icmp) {
	int32_t sum = 0;
	for (size_t i = 0; i < ICMP_PACKET_SIZE / 2; ++i) {
		sum += icmp[i];
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	sum = ~sum;
	return ((uint16_t)sum);
}

icmp_info_t init_icmp_info() {
	icmp_info_t icmp_info;
	icmp_info.pid = getpid();
	icmp_info.seq_nb = 1;
	return (icmp_info);
}

void update_icmp_info(icmp_info_t* icmp_info) {
	++(icmp_info->seq_nb);
}

void print_icmp(const struct icmp* icmp) {
	printf("\ntype: %hu code: %hu, pid: %d, seq: %d\n", icmp->icmp_type, icmp->icmp_code,
		   ntohs(icmp->icmp_id), ntohs(icmp->icmp_seq));
}
