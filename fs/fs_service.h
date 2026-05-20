#ifndef FS_SERVICE_H
#define FS_SERVICE_H

#include <types.h>

#define FS_NAME_MAX   64
#define FS_PATH_MAX   256
#define MAX_OPEN_FILES 32

typedef enum {
    FS_FILE,
    FS_DIR,
} fs_node_type_t;

typedef struct fs_node {
    char            name[FS_NAME_MAX];
    fs_node_type_t  type;
    uint32_t        size;
    uint32_t        inode;
    struct fs_node *parent;
    struct fs_node *children;
    struct fs_node *next;
} fs_node_t;

typedef struct {
    fs_node_t *node;
    uint32_t   offset;
    bool       open;
} file_handle_t;

/* Init VFS */
void fs_init(void);

/* Basic ops */
fs_node_t *fs_mkdir(const char *path);
fs_node_t *fs_create(const char *path);
fs_node_t *fs_find(const char *path);
int        fs_delete(const char *path);

/* File I/O */
int fs_open(const char *path);
int fs_close(int fd);
int fs_read(int fd, void *buf, uint32_t size);
int fs_write(int fd, const void *buf, uint32_t size);

/* Debug */
void fs_dump(fs_node_t *node, int depth);

#endif