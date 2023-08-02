TARGET = build/out

SRCS = src/sine.c src/system/graphics.c src/system/gamepad.c src/main.c \

CFLAGS = -Wall -Wpedantic -Wextra

PREFIX = /home/v360/Code/PSX/bin/mipsel-none-elf
include /home/v360/Code/PSX/home/nugget_psyq/nolibgs_hello_worlds/common.mk 
