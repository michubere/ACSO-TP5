
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    
    if (fs == NULL || strlen(pathname) == 0 || pathname == NULL) {
        return -1;
    }

    if (pathname[0] != '/') {
        return -1;
    }

    if (strcmp(pathname, "/") == 0) {
        return ROOT_INUMBER;
    }

    int inumber = ROOT_INUMBER;
    int length = strlen(pathname);
    char *name = malloc(length);
    if (name == NULL) {
        return -1;
    }

    const char *path_ptr = pathname + 1; // Skip the initial slash
    while (*path_ptr != '\0') {
        // Get the next pathname component
        int j = 0;
        while (*path_ptr != '/' && *path_ptr != '\0') {
            name[j++] = *path_ptr++;
        }
        name[j] = '\0'; // End the component with null

        // Find the inode number of the current component
        struct direntv6 dirEnt;
        int dir = directory_findname(fs, name, inumber, &dirEnt);
        if (dir == -1) {
            free(name);
            return -1;
        }

        // Update the inode number
        inumber = dirEnt.d_inumber;

        // Skip the slash if it exists
        if (*path_ptr == '/') {
            path_ptr++;
        }
    }

    // Free memory and return the final inode number
    free(name);
    return inumber;
}
