#include "include/platform.h"
#include "device/device.h"
#include "driver/driver_manager.h"
#include "fs/fs_service.h"
#include <hal/x86/serial/serial_debug.h>

extern module_t vga_module;
extern module_t serial_module;
extern module_t keyboard_module;
extern module_t ata_module;

static device_t vga_dev      = { .name = "vga0",  .type = DEV_DISPLAY };
static device_t serial_dev   = { .name = "ttyS0", .type = DEV_CHAR    };
static device_t keyboard_dev = { .name = "kbd0",  .type = DEV_CHAR    };
static device_t disk_dev     = { .name = "hda",   .type = DEV_BLOCK   };

int platform_init(void)
{
    serial_puts("\n[PLATFORM] Initializing...\n");

    device_init();
    driver_manager_init();

    device_register(&serial_dev);
    driver_register(&serial_module, &serial_dev);

    device_register(&vga_dev);
    driver_register(&vga_module, &vga_dev);

    device_register(&keyboard_dev);
    driver_register(&keyboard_module, &keyboard_dev);

    device_register(&disk_dev);
    driver_register(&ata_module, &disk_dev);

    fs_init();

    serial_puts("[PLATFORM] Ready\n");
    return 0;
}

void platform_dump(void)
{
    device_dump();
    driver_dump();
    serial_puts("[FS] VFS tree:\n");
    fs_dump(NULL, 0);
}