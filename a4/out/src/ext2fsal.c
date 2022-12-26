/*
 *------------
 * This code is provided solely for the personal and private use of
 * students taking the CSC369H5 course at the University of Toronto.
 * Copying for purposes other than this use is expressly prohibited.
 * All forms of distribution of this code, whether as given or with
 * any changes, are expressly prohibited.
 *
 * All of the files in this directory and all subdirectories are:
 * Copyright (c) 2022 MCS @ UTM
 * -------------
 */

#include "ext2fsal.h"
#include "e2fs.h"

#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>

pthread_mutex_t *inode_table_lock;
struct ext2_inode *inode_table;

pthread_mutex_t gd_lock;
struct ext2_group_desc *gd;

pthread_mutex_t sb_lock;
struct ext2_super_block *sb;


pthread_mutex_t block_bitmap_lock;
char *block_bitmap;

pthread_mutex_t inode_bitmap_lock;
char *inode_bitmap;

char *disk_image;

void ext2_fsal_init(const char* image)
{
    // Open the image
    int file = open(image, O_RDWR);

    // Map the image file to memory
    disk_image = mmap(NULL, BLOCK_COUNT * BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
    inode_table = (struct ext2_inode *)(gd->bg_inode_table * BLOCK_SIZE + disk_image);

    // Set up block_bitmap and its lock
    pthread_mutex_init(&block_bitmap_lock, NULL);
    block_bitmap = (char*)(gd->bg_block_bitmap * BLOCK_SIZE + disk_image);

    // Set up inode_bitmap and its lock
    pthread_mutex_init(&inode_bitmap_lock, NULL);
    inode_bitmap = (char*)(gd->bg_inode_bitmap * BLOCK_SIZE + disk_image);

    // Set up super block and its lock
    pthread_mutex_init(&sb_lock, NULL);
    sb = (struct ext2_super_block *)(BLOCK_SIZE + disk_image);

    // Set up group desc and its lock
    pthread_mutex_init(&gd_lock, NULL);
    gd = (struct ext2_group_desc *)(2 * BLOCK_SIZE + disk_image);

    // Set up locks for each inode in table
    int inode = 0;
    while (inode < sb->s_inodes_count) {
        pthread_mutex_init(&inode_table_lock[inode], NULL);
        inode++;
    }

}

void ext2_fsal_destroy()
{
    // Unmap disk image
    munmap(disk_image, BLOCK_COUNT * BLOCK_SIZE);

    // Delete locks for resources
    pthread_mutex_destroy(&block_bitmap_lock);
    pthread_mutex_destroy(&inode_bitmap_lock);
    pthread_mutex_destroy(&sb_lock);
    pthread_mutex_destroy(&gd_lock);

    // Delete locks for each inode in table
    int inode = 0;
    while (inode < sb->s_inodes_count) {
        pthread_mutex_destroy(&inode_table_lock[inode]);
        inode++;
    }
}