#pragma once

#include <stdbool.h> // -> bool, true false
#include <stddef.h>  // -> size_t, ptrdiff_t, NULL
#include <stdint.h>  // -> uint32_t, etc..

#include <libetc.h>  // -> VSync
#include <libgte.h>  // (needed for libgpu to not error)
#include <libgpu.h>  // -> DISPENV, DRAWENV

#include <stdio.h>   // -> printf
#include <stdlib.h>  // -> exit

// Length of the ordering table, i.e. the range Z coordinates can have, 0-15 in
// this case. Larger values will allow for more granularity with depth (useful
// when drawing a complex 3D scene) at the expense of RAM usage and performance.
#define OT_LAYERS 4

// Size of the buffer GPU commands and primitives are written to. If the program
// crashes due to too many primitives being drawn, increase this value.
#define BUFFER_LENGTH 8192

/* Framebuffer/display list class */

typedef struct {
	DISPENV disp_env;
	DRAWENV draw_env;

	uint32_t ord_tbl[OT_LAYERS];
	uint8_t  buffer[BUFFER_LENGTH];
} RenderBuffer;

typedef struct {
	RenderBuffer buffers[2];
	uint8_t     *next_packet;
	int          active_buffer;
} RenderContext;

void setup_context(RenderContext *ctx, int w, int h);

void set_clear_colors(RenderContext *ctx, uint8_t r, uint8_t g, uint8_t b);
void unset_clear_colors(RenderContext *ctx);

void flip_buffers(RenderContext *ctx);

void *new_primitive(RenderContext *ctx, int z, size_t size);
