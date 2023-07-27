/*
 * Me me made everything V360
// IM USING VER. 0.24 LAST I CHECKED
 * Based on PSn00bSDK basic graphics example
 * (C) 2020-2026 Lameguy64, spicyjpeg - MPL licensed
 */

#include <stdbool.h> // -> bool, true false
#include <stddef.h>  // -> size_t, ptrdiff_t, NULL
#include <stdint.h>  // -> uint32_t, etc..

#include <libetc.h>  // -> Fnt*, etc..
#include <libgte.h>  // geometry transformation engine
#include <libgpu.h>  // primitive structs and macros (needs defs from gte)
// #include <psxapi.h> // i dont need this; it's hardware shit

#include <stdio.h>  // -> printf
#include <stdlib.h> // -> exit
#include <string.h> // -> memcpy, memset
#include <assert.h> // -> assert

#include "fixed_point.h"

#include "context.h" // -> RenderContext

#include "sine.h"    // -> isin, icos

#define sizeofarr(x) (sizeof(x) / sizeof(*(x)))

// A simple helper for drawing text using PSn00bSDK's debug font API. Note that
// FntSort() requires the debug font texture to be uploaded to VRAM beforehand
// by calling FntLoad().
void draw_text(RenderContext *ctx, int x, int y, int z, const char *text) {
	RenderBuffer *buffer = &(ctx->buffers[ctx->active_buffer]);

	// Pretty much loops over the character buffer and makes a sprite packet
	// for each character. Yes it spends your freaking packet budget...

	// ctx->next_packet =
	// 	(uint8_t *)FntSort(&(buffer->ord_tbl[z]), ctx->next_packet, x, y, text);

	assert(ctx->next_packet <= &(buffer->buffer[BUFFER_LENGTH]));
}

/* Main */

#define SCREEN_SIZE_X 320
#define SCREEN_SIZE_Y 240

typedef struct {
	int x, y;
	int sx, sy;
	int dx, dy;
} bounce_box;

void setup_box(bounce_box *b) {
	b->x = b->y = 0;
	b->sx = b->sy = 64;
	b->dx = b->dy = 1;
}

void update_box(bounce_box *b) {
	if (b->x < 0 || b->x >= (SCREEN_SIZE_X - b->sx)) b->dx = -b->dx;
	if (b->y < 0 || b->y >= (SCREEN_SIZE_Y - b->sy)) b->dy = -b->dy;

	b->x += b->dx;
	b->y += b->dy;
}

void draw_box(bounce_box *b, RenderContext *ctx) {
	TILE *tile = (TILE *)new_primitive(ctx, 2, sizeof(TILE));

	setTile(tile);
	setXY0(tile, b->x, b->y);
	setWH(tile, b->sx, b->sy);
	setRGB0(tile, 255, 255, 0);
}

int main(int argc, const char **argv) {
	// Make a freaking render context.
	RenderContext ctx;

	// Initialize the GPU and associated interrupts.
	ResetGraph(0);

	// Load the default font texture provided by [whoever] at (960, 0) in VRAM.
	FntLoad(960, 0);

	// Set up our rendering context.
	setup_context(&ctx, SCREEN_SIZE_X, SCREEN_SIZE_Y);
	set_clear_colors(&ctx, 63, 0, 127);

	bounce_box box;
	setup_box(&box);

	int frames = 0;

	while (true) {
		// Update the position and velocity of the bouncing square.
		update_box(&box);

		// Draw the square by allocating a TILE (i.e. untextured solid color
		// rectangle) primitive at Z = 1.
		draw_box(&box, &ctx);

		POLY_G3 *triangle = (POLY_G3 *)new_primitive(&ctx, 2, sizeof(POLY_G3));

		int tri_x = icos(frames << 6) >> 9;
		int tri_y = isin(frames << 5) >> 9;

		setPolyG3(triangle);
		setXY3(triangle,
			/**/ 48 + tri_x, 32 + tri_y,
			/**/ 32 - tri_x, 48 + tri_y,
			/**/ 64 + tri_x, 48 - tri_y);
		setRGB0(triangle, 255, 0, 0);
		setRGB1(triangle, 0, 255, 0);
		setRGB2(triangle, 0, 0, 255);

		int oscillate = isin(frames << 7) >> 10;

		POLY_FT4 *quad = (POLY_FT4 *)new_primitive(&ctx, 2, sizeof(POLY_FT4));

		setPolyFT4(quad);
		setXY4(quad,
			/**/ 24 + oscillate, 16,
			/**/ SCREEN_SIZE_X / 2, 16,
			/**/ 16 + oscillate, SCREEN_SIZE_Y - 16,
			/**/ SCREEN_SIZE_X / 2, SCREEN_SIZE_Y - 16);
		setRGB0(quad, 192, 192, 192);
		setTPage(quad, 2, 1, SCREEN_SIZE_X, 0);
		setUV4(quad,
			/**/ 0, 0,
			/**/ SCREEN_SIZE_X / 2, 0,
			/**/ 0, 239,
			/**/ SCREEN_SIZE_X / 2, 239);

		quad = (POLY_FT4 *)new_primitive(&ctx, 2, sizeof(POLY_FT4));
		int right_half_ofs =
			(SCREEN_SIZE_X / 2) - ((SCREEN_SIZE_X / 2 / 64) * 64);

		setPolyFT4(quad);
		setXY4(quad,
			/**/ SCREEN_SIZE_X / 2, 16,
			/**/ SCREEN_SIZE_X - (24 + oscillate), 16,
			/**/ SCREEN_SIZE_X / 2, SCREEN_SIZE_Y - 16,
			/**/ SCREEN_SIZE_X - (16 + oscillate), SCREEN_SIZE_Y - 16);
		setRGB0(quad, 192, 192, 192);
		setTPage(quad, 2, 1, SCREEN_SIZE_X + SCREEN_SIZE_X / 2, 0);
		setUV4(quad,
			/**/ right_half_ofs, 0,
			/**/ right_half_ofs + SCREEN_SIZE_X / 2, 0,
			/**/ right_half_ofs, 239,
			/**/ right_half_ofs + SCREEN_SIZE_X / 2, 239);

		DR_MOVE *last_frame = (DR_MOVE *)new_primitive(
			&ctx, 0, sizeof(DR_MOVE) + sizeof(uint32_t[4]));
		RECT source_rect;
		source_rect.x = 0;
		source_rect.y = (ctx.active_buffer) * SCREEN_SIZE_Y;
		source_rect.w = SCREEN_SIZE_X;
		source_rect.h = SCREEN_SIZE_Y;
		SetDrawMove(last_frame, &source_rect, SCREEN_SIZE_X, 0);

		// Draw some text in front of the square (Z = 0, primitives with higher
		// Z indices are drawn first and thus drawn "behind" this).
		draw_text(&ctx, 8, 16, 0, "Hello, world 2!");
		char clock_buff[8];
		sprintf(clock_buff, /* sizeofarr(clock_buff), */ "c: %04d", frames);
		draw_text(&ctx, 12, 24, 0, clock_buff);

		flip_buffers(&ctx);

		frames++;
	}

	return 0;
}
