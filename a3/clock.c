/*
 * This code is provided solely for the personal and private use of students
 * taking the CSC369H course at the University of Toronto. Copying for purposes
 * other than this use is expressly prohibited. All forms of distribution of
 * this code, including but not limited to public repositories on GitHub,
 * GitLab, Bitbucket, or any other online platform, whether as given or with
 * any changes, are expressly prohibited.
 *
 * Authors: Bogdan Simion, Andrew Peterson, Karen Reid, Alexey Khrabrov, Vladislav Sytchenko
 *
 * All of the files in this directory and all subdirectories are:
 * Copyright (c) 2022 Bogdan Simion, Karen Reid
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pagetable.h"

extern int memsize;
extern bool debug;
extern struct frame *coremap;
static int evict;
static int frame;
/* The page to evict is chosen using the CLOCK algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	while (evict < memsize) { // rotate eviction pointer until we have a victim
		frame = (coremap[evict].pte)->frame;
		if (PG_REFERENCED(frame)) { // if reference bit is set (protection)
			(coremap[evict].pte)->frame = PG_DEREFERENCE(frame); // unset it
		} else { // if reference bit is not set (not protected)
			return evict; // evict
		}
		evict += 1;
		if (evict == memsize) { // infinite loop until we assign a victim
			evict = 0;
		}
	}
    return 0;
}

/* This function is called on every access to a page to update any information
 * needed by the CLOCK algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pt_entry_t *pte) {
	(void)pte;
}

/* Initialize any data structures needed for this replacement
 * algorithm.
 */
void clock_init() {
	evict = 0;
	frame = 0;
}

/* Cleanup any data structures created in clock_init(). */
void clock_cleanup() {
}
