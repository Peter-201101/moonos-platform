#ifndef PLATFORM_H
#define PLATFORM_H

#include <types.h>
#include <module.h>

#define PLATFORM_OK   0
#define PLATFORM_ERR -1

int  platform_init(void);
void platform_dump(void);

#endif