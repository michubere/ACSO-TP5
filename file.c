#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    
    // if (blockNum < 0 || blockNum >= 8 || inumber < 1 || inumber > fs->superblock.s_ninode || buf == NULL || fs == NULL) {
    //     return -1;
    // }

    struct inode inp;
    int inode = inode_iget(fs, inumber, &inp);
    if (inode == -1) {
        return -1;
    }

    int disk_block = inode_indexlookup(fs, &inp, blockNum);
    if (disk_block == -1) {
        return -1;
    }

    int read = diskimg_readsector(fs->dfd, disk_block, buf);
    if (read == -1) {
        return -1;
    }

    int bytes = inode_getsize(&inp);
    if (bytes < 0) {
        return -1;
    }

    int blocks = bytes / DISKIMG_SECTOR_SIZE;
    if (blockNum == blocks) {
        return bytes % DISKIMG_SECTOR_SIZE;
    } else {
        return DISKIMG_SECTOR_SIZE;
    }
    return 0;
}

