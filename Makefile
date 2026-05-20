BUILD  ?= build
CC      = gcc
AR      = ar

CFLAGS  = -m32 -std=c99 -ffreestanding -fno-stack-protector  \
          -fno-pie -fno-pic -fno-builtin                      \
          -O2 -Wall -Wextra -nostdinc                         \
          -I.                                                  \
          -I./include                                          \
          -I../moonos-kernel/include                           \
          -I../moonos-kernel/lib                               \
          -I../moonos-kernel                                   \
          -I../moonos-modules/include                          \
          -I../moonos-modules

C_SRCS  = platform.c              \
          device/device.c         \
          driver/driver_manager.c \
          fs/fs_service.c

C_OBJS  = $(patsubst %.c, $(BUILD)/%.o, $(C_SRCS))
OUTPUT  = $(BUILD)/moonos-platform.a

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(C_OBJS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^
	@echo "[OK] Platform: $@"

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "[CC] $<"

clean:
	rm -rf $(BUILD)