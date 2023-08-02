#pragma once

#include "../types.h" // -> integer, boolean, pointer

#include <libetc.h>   // -> VSync
#include <libgte.h>   // (needed for libgpu to not error)
#include <libgpu.h>   // -> DISPENV, DRAWENV

#include <stdio.h>    // -> printf
#include <stdlib.h>   // -> exit

// Length of the ordering table, i.e. the range Z coordinates can have, 0-7 in
// this case. Larger values will allow for more granularity with depth (useful
// when drawing a complex 3D scene) at the expense of RAM usage and performance.
#define OT_LAYERS 8

// Size of the buffer GPU commands and primitives are written to. If the program
// crashes due to too many primitives being drawn, increase this value.
#define BUFFER_LENGTH 8192

/* Framebuffer/display list class */

typedef struct {
	DISPENV disp_env;
	DRAWENV draw_env;

	u32 ord_tbl[OT_LAYERS];
	u8  buffer[BUFFER_LENGTH];
} RenderBuffer;

typedef struct {
	RenderBuffer buffers[2];
	void        *next_packet;
	void        *packet_buffer_end;
	u8           active_buffer;
} RenderContext;

void setup_context(RenderContext *ctx, int w, int h);

void set_clear_colors(RenderContext *ctx, u8 r, u8 g, u8 b);
void unset_clear_colors(RenderContext *ctx);

void flip_buffers(RenderContext *ctx);

void *new_primitive(RenderContext *ctx, int z, usize size);
