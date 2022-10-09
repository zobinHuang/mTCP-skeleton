/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/queue.h>

#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>
#include <rte_ethdev.h>

int main(int argc, char **argv)
{
	int cnt_args_parsed;
	uint32_t id_core;
	uint32_t cnt_ports;

	/* Init runtime environment */
	cnt_args_parsed = rte_eal_init(argc, argv);
	if (cnt_args_parsed < 0)
		rte_exit(EXIT_FAILURE, "rte_eal_init(): Failed");

	cnt_ports = rte_eth_dev_count_avail();
	printf("Number of NICs: %i\n", cnt_ports);

	/* clean up the EAL */
	rte_eal_cleanup();

	return 0;
}