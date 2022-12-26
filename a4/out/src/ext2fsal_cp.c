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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


int32_t ext2_fsal_cp(const char *src,
                     const char *dst)
{
    /*

Case 1: Destination path valid, destination is not an existing file or directory, need to create file with destination name and copy data into it
Arguments: "doh", "/foo/bar/blah", where the target path is valid and blah does not already exist. This will copy the source file "doh" into a new file called "blah" in the image in the indicated path.

Case 2: Destination path invalid because one of the directories is not actually a directory
Arguments: "doh", "/foo/bar/blah", where either foo or bar does not exist, or either foo or bar is a file. This returns ENOENT.

Case 3: Destination path valid, destination name is an existing file, need to overwrite that file
Arguments: "doh", "/foo/bar/blah", where the target path is valid but file "blah" exists. This will overwrite the file in the image.

Group case 4 & case 5:

Case 4: Destination path valid, destination name is an existing directory (even though it doesn't end with '/'), need to create a file with original name and copy data into it
Arguments: "doh" "/foo/bar/blah", where the target path is valid but "blah" is a directory. The file "doh" will be copied as "doh" under directory "blah".

Group below 2 cases:

Case 5: Destination path is valid and is a directory (ends with a '/'), copy original name and file under the directory
Arguments: "doh" "/foo/bar/blah/", where the target path is valid and a file "doh" does not already exist under "blah". The file "doh" will be copied under directory "blah".

Case 6: Destination path is valid, destination is a directory because it ends with a '/' and original file name already exists within it, need to overwrite that file.
Arguments: "doh" "/foo/bar/blah/", where the path is valid and a file "doh" already exists under "blah". The file "doh" will be overwritten in the image.

*/

    (void)src;
    (void)dst;
    
    return 0;
}