#ifndef DRIVER_MANAGER_H
#define DRIVER_MANAGER_H

#include <types.h>
#include <module.h>
#include "device/device.h"

#define MAX_DRIVERS 32

typedef struct driver {
    char        name[32];
    module_t   *module;
    device_t   *device;     /* device yang di-handle driver ini */
    bool        loaded;
} driver_t;

void      driver_manager_init(void);
int       driver_register(module_t *mod, device_t *dev);
int       driver_unregister(const char *name);
driver_t *driver_get(const char *name);
void      driver_dump(void);

#endif