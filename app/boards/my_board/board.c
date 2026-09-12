/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

static int my_board_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(my_board_init, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY);
