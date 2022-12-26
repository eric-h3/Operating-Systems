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

/* The page to evict is chosen using the ARC algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int arc_evict() {

        return 0;
}

/* This function is called on each access to a page to update any information
 * needed by the ARC algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void arc_ref(pt_entry_t *pte) {

	// This is keep the compiler happy, until you implement this.
	(void)pte;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void arc_init() {
}

/* Cleanup any data structures created in arc_init(). */
void arc_cleanup() {
}

