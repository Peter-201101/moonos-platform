#include "device.h"
#include <hal/x86/serial/serial_debug.h>
#include <lib/string.h>

static device_t *devices[MAX_DEVICES];
static uint32_t  device_count = 0;
static uint32_t  next_id      = 1;

void device_init(void)
{
    for (uint32_t i = 0; i < MAX_DEVICES; i++)
        devices[i] = NULL;
    device_count = 0;
    serial_puts("[DEV] Device manager initialized\n");
}

int device_register(device_t *dev)
{
    if (!dev) return -1;
    if (device_count >= MAX_DEVICES) {
        serial_puts("[DEV] Max devices reached\n");
        return -1;
    }

    dev->id     = next_id++;
    dev->active = true;

    for (uint32_t i = 0; i < MAX_DEVICES; i++) {
        if (!devices[i]) {
            devices[i] = dev;
            device_count++;
            serial_printf("[DEV] Registered: [%u] %s\n", dev->id, dev->name);
            return 0;
        }
    }

    return -1;
}

int device_unregister(uint32_t id)
{
    for (uint32_t i = 0; i < MAX_DEVICES; i++) {
        if (devices[i] && devices[i]->id == id) {
            serial_printf("[DEV] Unregistered: [%u] %s\n",
                devices[i]->id, devices[i]->name);
            devices[i]->active = false;
            devices[i] = NULL;
            device_count--;
            return 0;
        }
    }
    return -1;
}

device_t *device_get(uint32_t id)
{
    for (uint32_t i = 0; i < MAX_DEVICES; i++)
        if (devices[i] && devices[i]->id == id)
            return devices[i];
    return NULL;
}

device_t *device_get_by_name(const char *name)
{
    for (uint32_t i = 0; i < MAX_DEVICES; i++)
        if (devices[i] && strcmp(devices[i]->name, name) == 0)
            return devices[i];
    return NULL;
}

void device_dump(void)
{
    static const char *type_str[] = {
        "UNKNOWN", "CHAR", "BLOCK", "DISPLAY", "NETWORK"
    };

    serial_puts("[DEV] === Device list ===\n");
    for (uint32_t i = 0; i < MAX_DEVICES; i++) {
        if (devices[i]) {
            serial_printf("[DEV] [%u] %-16s type=%s\n",
                devices[i]->id,
                devices[i]->name,
                type_str[devices[i]->type]);
        }
    }
    serial_printf("[DEV] Total: %u devices\n", device_count);
    serial_puts("[DEV] ====================\n");
}