ifndef PSX_PREFIX_DIR
$(error PSX_PREFIX_DIR is not set. gimme it without the trailing slash)
endif

TARGET_DIR = build
TARGET = $(TARGET_DIR)/out

ISO_TARGET_DIR = $(TARGET_DIR)/iso
ISO_TARGET = $(ISO_TARGET_DIR)/out

SRCS = \
	src/system/graphics.c src/system/gamepad.c \
	src/helper/sine.c \
	src/player.c \
	src/main.c

CFLAGS = -Wall -Wpedantic -Wextra

PREFIX = $(PSX_PREFIX_DIR)/bin/mipsel-none-elf
include $(PSX_PREFIX_DIR)/home/nugget_psyq/nolibgs_hello_worlds/common.mk 

.PHONY: iso
iso:
	mkdir -p $(ISO_TARGET_DIR)
	$(PSX_PREFIX_DIR)/bin/mkpsxiso -o $(ISO_TARGET).bin -c $(ISO_TARGET).cue -y iso.xml
