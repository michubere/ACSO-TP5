#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

#define INDIR_ADDR 7

/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    // if (inumber < 1 || inumber > fs->superblock.s_ninode || inp == NULL || fs == NULL) {
    //     return -1;
    // }

    int inodes_per_block = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int block_num = (inumber - 1) / inodes_per_block;
    int offset = (inumber - 1) % inodes_per_block;

    struct inode inodes[inodes_per_block];
    int read = diskimg_readsector(fs->dfd, INODE_START_SECTOR + offset, inodes);
    if (read == -1) {
        return -1;
    }

    *inp = inodes[offset];

    return 0; 
}

/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp,
    int blockNum) {  
    // if (blockNum < 0 || blockNum >= 8 || inp == NULL || fs == NULL) {
    //     return -1;
    // }

    // small file
    if ((inp->i_mode & ILARG) == 0) {
        return inp->i_addr[blockNum];
    }

    // large file
    int address_number = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);
    int indirect_address_number = address_number * INDIR_ADDR;
    if (blockNum < indirect_address_number) {
        int offset = blockNum / address_number;
        uint16_t address[address_number];
        int read = diskimg_readsector(fs->dfd, inp->i_addr[offset], address);
        if (read == -1) {
            return -1;
        }

        return address[blockNum % address_number];
    } else {
        // layer one
        int blockNum_double = blockNum - indirect_address_number;
        int offset1 = blockNum_double / address_number;
        uint16_t address1[address_number];
        int read1 = diskimg_readsector(fs->dfd, inp->i_addr[offset1], address1);
        if (read1 == -1) {
            return -1;
        }

        // layer two
        int offset2 = blockNum_double % address_number;
        uint16_t address2[address_number];
        int read2 = diskimg_readsector(fs->dfd, address1[offset1], address2);
        if (read2 == -1) {
            return -1;
        }
        return address2[offset2];
    }

    return 0;
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
