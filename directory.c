#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DIRENTV6_NAMELEN 14
/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *name,
		int dirinumber, struct direntv6 *dirEnt) {
  
  if (fs == NULL || strlen(name) == 0 || strlen(name) > DIRENTV6_NAMELEN || dirinumber < 1 || dirinumber > fs->superblock.s_ninodes || dirEnt == NULL) {
    return -1;
  }

  struct inode inp;
  int inode = inode_iget(fs, dirinumber, &inp);
  if (inode == -1) {
    return -1;
  }

  if ((inp.i_mode & IFMT) != IFDIR) {
    return -1;
  }

  int bytes = inode_getsize(&inp);
  if (bytes < 0 || bytes == 0) {
    return -1;
  }

  int blocks = (bytes - 1) / DISKIMG_SECTOR_SIZE + 1;
  for (int i = 0; i < blocks; i++) {
    struct direntv6 buf[DISKIMG_SECTOR_SIZE / sizeof(struct direntv6)];
    int block = file_getblock(fs, dirinumber, i, buf);
    if (block == -1) {
      return -1;
    }

    int num = block / sizeof(struct direntv6);
    for (int j = 0; j < num; j++) {
      if (strncmp(dir[j].d_name, name) == 0) {
        *dirEnt = buf[j];
        return 0;
      }
    }
  }
  return -1;
}
