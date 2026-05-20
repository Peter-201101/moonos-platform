#ifndef DEVICE_H
#define DEVICE_H

#include <types.h>

#define DEVICE_NAME_MAX 32
#define MAX_DEVICES     64

typedef enum {
    DEV_UNKNOWN = 0,
    DEV_CHAR,       /* character device: serial, keyboard */
    DEV_BLOCK,      /* block device: disk */
    DEV_DISPLAY,    /* display */
    DEV_NETWORK,    /* network */
} device_type_t;

typedef struct device {
    uint32_t        id;
    char            name[DEVICE_NAME_MAX];
    device_type_t   type;
    bool            active;
    void           *driver_data;    /* pointer ke driver private data */

    /* ops */
    int  (*open)(struct device *dev);
    int  (*close)(struct device *dev);
    int  (*read)(struct device *dev, void *buf, uint32_t size);
    int  (*write)(struct device *dev, const void *buf, uint32_t size);
    int  (*ioctl)(struct device *dev, uint32_t cmd, void *arg);
} device_t;

void      device_init(void);
int       device_register(device_t *dev);
int       device_unregister(uint32_t id);
device_t *device_get(uint32_t id);
device_t *device_get_by_name(const char *name);
void      device_dump(void);

#endif