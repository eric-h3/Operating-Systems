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
int current_timestamp;
int smallest;
int current_frame;

/* The page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	// parse pages and decide which one was used the oldest (smallest timestamp wins)
	smallest = current_timestamp + 1;
	for (int i = 0; i < memsize; i++) {
		if (coremap[i].timestamp < smallest) {
			evict = i;
			smallest = coremap[i].timestamp;
		}
	}
    return evict;
}

/* This function is called on each access to a page to update any information
 * needed by the LRU algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pt_entry_t *pte) {
	if (PAGE_VALID == 0) {
		return;
	}
	current_frame = pte->frame >> PAGE_SHIFT;
	if (coremap[current_frame].in_use) {
		current_timestamp += 1;
		coremap[current_frame].timestamp = current_timestamp;
	}
	return;
}


/* Initialize any data structures needed for this
 * replacement algorithm
 */
void lru_init() {
	evict = 0;
	current_timestamp = 0;
	smallest = current_timestamp;
	current_frame = 0;
	for (int i = 0; i < memsize; i++) {
		coremap[i].timestamp = 0;
	}
}

/* Cleanup any data structures created in lru_init(). */
void lru_cleanup() {
}

