#include "fs_service.h"
#include <core/memory.h>
#include <hal/x86/serial/serial_debug.h>
#include <lib/string.h>

/* Root node */
static fs_node_t  *fs_root   = NULL;
static file_handle_t open_files[MAX_OPEN_FILES];
static uint32_t   next_inode = 1;

/*---------------------------------------------------------------------------
 * Internal
 *---------------------------------------------------------------------------*/

static fs_node_t *node_create(const char *name, fs_node_type_t type)
{
    fs_node_t *node = (fs_node_t *)kmalloc(sizeof(fs_node_t));
    if (!node) return NULL;

    strncpy(node->name, name, FS_NAME_MAX - 1);
    node->name[FS_NAME_MAX - 1] = '\0';
    node->type     = type;
    node->size     = 0;
    node->inode    = next_inode++;
    node->parent   = NULL;
    node->children = NULL;
    node->next     = NULL;

    return node;
}

static void node_add_child(fs_node_t *parent, fs_node_t *child)
{
    child->parent = parent;
    if (!parent->children) {
        parent->children = child;
        return;
    }

    fs_node_t *cur = parent->children;
    while (cur->next) cur = cur->next;
    cur->next = child;
}

/* Parse path dan traverse ke node */
static fs_node_t *path_resolve(const char *path, fs_node_t **parent_out)
{
    if (!path || path[0] != '/') return NULL;

    fs_node_t *cur    = fs_root;
    fs_node_t *parent = NULL;

    if (strcmp(path, "/") == 0) {
        if (parent_out) *parent_out = NULL;
        return fs_root;
    }

    /* Copy path untuk tokenize */
    char tmp[FS_PATH_MAX];
    strncpy(tmp, path + 1, FS_PATH_MAX - 1); /* skip leading / */

    char *token = tmp;
    char *next  = NULL;

    while (token && *token) {
        /* Find next slash */
        next = token;
        while (*next && *next != '/') next++;
        if (*next == '/') { *next = '\0'; next++; }
        else next = NULL;

        /* Cari nama di children */
        fs_node_t *found = NULL;
        fs_node_t *child = cur->children;
        while (child) {
            if (strcmp(child->name, token) == 0) {
                found = child;
                break;
            }
            child = child->next;
        }

        if (!found) {
            if (parent_out) *parent_out = cur;
            return NULL;
        }

        parent = cur;
        cur    = found;
        token  = next;
    }

    if (parent_out) *parent_out = parent;
    return cur;
}

/* Ambil nama file dari path */
static const char *path_basename(const char *path)
{
    const char *base = path;
    while (*path) {
        if (*path == '/') base = path + 1;
        path++;
    }
    return base;
}

/*---------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

void fs_init(void)
{
    fs_root = node_create("/", FS_DIR);

    /* Buat direktori standar */
    fs_mkdir("/bin");
    fs_mkdir("/etc");
    fs_mkdir("/dev");
    fs_mkdir("/tmp");
    fs_mkdir("/home");
    fs_mkdir("/var");
    fs_mkdir("/var/log");

    for (uint32_t i = 0; i < MAX_OPEN_FILES; i++)
        open_files[i].open = false;

    serial_puts("[FS] VFS initialized\n");
}

fs_node_t *fs_mkdir(const char *path)
{
    fs_node_t *parent = NULL;
    fs_node_t *exists = path_resolve(path, &parent);
    if (exists) return exists;
    if (!parent) parent = fs_root;

    const char *name = path_basename(path);
    fs_node_t  *node = node_create(name, FS_DIR);
    if (!node) return NULL;

    node_add_child(parent, node);
    return node;
}

fs_node_t *fs_create(const char *path)
{
    fs_node_t *parent = NULL;
    fs_node_t *exists = path_resolve(path, &parent);
    if (exists) return exists;
    if (!parent) parent = fs_root;

    const char *name = path_basename(path);
    fs_node_t  *node = node_create(name, FS_FILE);
    if (!node) return NULL;

    node_add_child(parent, node);
    return node;
}

fs_node_t *fs_find(const char *path)
{
    return path_resolve(path, NULL);
}

int fs_delete(const char *path)
{
    fs_node_t *parent = NULL;
    fs_node_t *node   = path_resolve(path, &parent);
    if (!node || !parent) return -1;

    /* Remove dari linked list parent */
    if (parent->children == node) {
        parent->children = node->next;
    } else {
        fs_node_t *cur = parent->children;
        while (cur && cur->next != node) cur = cur->next;
        if (cur) cur->next = node->next;
    }

    kfree(node);
    return 0;
}

int fs_open(const char *path)
{
    fs_node_t *node = fs_find(path);
    if (!node) return -1;

    for (uint32_t i = 0; i < MAX_OPEN_FILES; i++) {
        if (!open_files[i].open) {
            open_files[i].node   = node;
            open_files[i].offset = 0;
            open_files[i].open   = true;
            return (int)i;
        }
    }

    return -1; /* too many open files */
}

int fs_close(int fd)
{
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    open_files[fd].open = false;
    return 0;
}

int fs_read(int fd, void *buf, uint32_t size)
{
    (void)buf; (void)size;
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    if (!open_files[fd].open) return -1;
    /* TODO: actual data storage */
    return 0;
}

int fs_write(int fd, const void *buf, uint32_t size)
{
    (void)buf; (void)size;
    if (fd < 0 || fd >= MAX_OPEN_FILES) return -1;
    if (!open_files[fd].open) return -1;
    /* TODO: actual data storage */
    return (int)size;
}

void fs_dump(fs_node_t *node, int depth)
{
    if (!node) node = fs_root;

    for (int i = 0; i < depth; i++) serial_puts("  ");
    serial_printf("%s%s\n",
        node->name,
        node->type == FS_DIR ? "/" : "");

    if (node->type == FS_DIR) {
        fs_node_t *child = node->children;
        while (child) {
            fs_dump(child, depth + 1);
            child = child->next;
        }
    }
}