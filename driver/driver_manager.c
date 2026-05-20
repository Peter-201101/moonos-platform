#include "driver_manager.h"
#include <hal/x86/serial/serial_debug.h>
#include <lib/string.h>

static driver_t drivers[MAX_DRIVERS];
static uint32_t driver_count = 0;

void driver_manager_init(void)
{
    for (uint32_t i = 0; i < MAX_DRIVERS; i++) {
        drivers[i].loaded = false;
        drivers[i].module = NULL;
        drivers[i].device = NULL;
    }
    driver_count = 0;
    serial_puts("[DRV] Driver manager initialized\n");
}

int driver_register(module_t *mod, device_t *dev)
{
    if (!mod) return -1;
    if (driver_count >= MAX_DRIVERS) return -1;

    for (uint32_t i = 0; i < MAX_DRIVERS; i++) {
        if (!drivers[i].loaded) {
            strncpy(drivers[i].name, mod->name, 31);
            drivers[i].module = mod;
            drivers[i].device = dev;
            drivers[i].loaded = true;

            /* Init module */
            if (mod->init) {
                int ret = mod->init();
                if (ret != MODULE_OK) {
                    serial_printf("[DRV] Module init failed: %s\n", mod->name);
                    drivers[i].loaded = false;
                    return -1;
                }
            }

            mod->state = MODULE_READY;
            driver_count++;

            serial_printf("[DRV] Loaded: %s v%s\n", mod->name, mod->version);
            return 0;
        }
    }

    return -1;
}

int driver_unregister(const char *name)
{
    for (uint32_t i = 0; i < MAX_DRIVERS; i++) {
        if (drivers[i].loaded && strcmp(drivers[i].name, name) == 0) {
            if (drivers[i].module && drivers[i].module->exit)
                drivers[i].module->exit();

            drivers[i].module->state = MODULE_UNLOADED;
            drivers[i].loaded = false;
            driver_count--;

            serial_printf("[DRV] Unloaded: %s\n", name);
            return 0;
        }
    }
    return -1;
}

driver_t *driver_get(const char *name)
{
    for (uint32_t i = 0; i < MAX_DRIVERS; i++)
        if (drivers[i].loaded && strcmp(drivers[i].name, name) == 0)
            return &drivers[i];
    return NULL;
}

void driver_dump(void)
{
    serial_puts("[DRV] === Driver list ===\n");
    for (uint32_t i = 0; i < MAX_DRIVERS; i++) {
        if (drivers[i].loaded) {
            serial_printf("[DRV] %-16s state=%s\n",
                drivers[i].name,
                drivers[i].module->state == MODULE_READY ? "READY" : "ERROR");
        }
    }
    serial_printf("[DRV] Total: %u drivers\n", driver_count);
    serial_puts("[DRV] ====================\n");
}