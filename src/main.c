/*
 * Me me made everything V360
// IM USING VER. 0.24 LAST I CHECKED
 * Based on PSn00bSDK basic graphics example
 * (C) 2020-2026 Lameguy64, spicyjpeg - MPL licensed
 */

#include "types.h"

#include <libetc.h>  // -> Fnt*, etc..
#include <libgte.h>  // geometry transformation engine
#include <libgpu.h>  // primitive structs and macros

// libgpu needs stuff from libgte and some antiquated types from
// sys/types (see https://stackoverflow.com/a/1918960/).

// #include <psxapi.h> // i dont need this; it's hardware shit

#include <stdio.h>  // -> printf
#include <stdlib.h> // -> exit
#include <string.h> // -> memcpy, memset
#include <assert.h> // -> assert

#include "fixed_point.h"

#include "system/gamepad.h"     // -> GamepadState
#include "system/graphics.h"    // -> RenderContext

#include "helper/sine.h" // -> isin, icos

// A simple helper for drawing text using [whatever]'s debug font API. Note that
// FntSort() requires the debug font texture to be uploaded to VRAM beforehand
// by calling FntLoad().
void draw_text(RenderContext *ctx, int x, int y, int z, const char *text) {
	// The default font is stored as 4-bit sprites. Examples typically put the
	// texture at (960, 0). The FntLoad function also loads the needed CLUT
	// (consisting entirely of transparent and white entries only) and places
	// it 128 pixels below the specified position, so typically (960, 128).
	//
	// The texture is 16x8 tiles, each 8x8 pixel squares. The texture starts
	// with the first printable character, the space. Yea

	// TODO: can i like. find the font global state and use that
	// instead of hard-coding the texture's position like this?
	short clut = GetClut(960, 0 + 128);

	// Pretty much loops over the character buffer and makes a sprite packet
	// for each character. Yes it spends your freaking packet budget...

	usize lx = x, ly = y;
	for (const char *curr = text; *curr != '\0'; curr++) {
		char c = *curr;

		// Line break
		if (c == '\n') {
			lx = x;
			ly += 8;
			continue;
		}

		// Tab stop
		if (c == '\t') {
			lx -= x;
			const int tab_stop = 4;
			lx = ((lx >> (tab_stop + 2)) + 1) << (tab_stop + 2);
			lx += x;
			continue;
		}

		// All caps
		if (c >= 'a' && c <= 'z') c -= ('a' - 'A');

		if (c >= ' ' && c <= '_') {
			c -= ' ';

			SPRT_8 *spr = (SPRT_8 *)new_primitive(ctx, z, sizeof(SPRT_8));

			setSprt8(spr);
			setShadeTex(spr, 1);
			setSemiTrans(spr, 0);
			// setRGB0(spr, 255, 255, 255);
			setXY0(spr, lx, ly);
			int u = (c % 16) * 8;
			int v = (c / 16) * 8;
			setUV0(spr, u, v);
			spr->clut = clut;
		}

		lx += 8;
	}

	DR_TPAGE *tp = (DR_TPAGE *)new_primitive(ctx, z, sizeof(DR_TPAGE));
	setDrawTPage(tp, 0, 0, 15);
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
	if (b->x < 0) b->dx = +1;
	if (b->y < 0) b->dy = +1;

	if (b->x >= (SCREEN_SIZE_X - b->sx)) b->dx = -1;
	if (b->y >= (SCREEN_SIZE_Y - b->sy)) b->dy = -1;

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

int main(void) {
	// Make and set up a controllers listener.
	GamepadState ctl;
	setup_gamepad(&ctl);

	// Make a rendering context.
	RenderContext ctx;

	// Before setting anything up, first
	// initialize the GPU and associated interrupts.
	ResetGraph(0);

	// Set up the rendering context.
	setup_context(&ctx, SCREEN_SIZE_X, SCREEN_SIZE_Y);
	set_clear_colors(&ctx, 63, 0, 127);

	// Load the default font texture into (960, 0) in VRAM.
	FntLoad(960, 0);

	bounce_box box;
	setup_box(&box);

	int frames = 0;

	while (true) {
		update_gamepad(&ctl);

		// Update the position and velocity of the bouncing square.
		update_box(&box);

		// Draw the square by allocating a TILE primitive at Z = 1.
		// (Tiles are just untextured solid-color rectangles)
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

		int oscillate = isin(frames << 7) >> 11;

		POLY_FT4 *quad = (POLY_FT4 *)new_primitive(&ctx, 2, sizeof(POLY_FT4));

		setPolyFT4(quad);
		setXY4(quad,
			/**/ 24 + oscillate, 16,
			/**/ SCREEN_SIZE_X / 2, 16,
			/**/ 16 + oscillate, SCREEN_SIZE_Y - 16,
			/**/ SCREEN_SIZE_X / 2, SCREEN_SIZE_Y - 16);
		setRGB0(quad, 160, 160, 160);
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
		setRGB0(quad, 160, 160, 160);
		setTPage(quad, 2, 1, SCREEN_SIZE_X + SCREEN_SIZE_X / 2, 0);
		setUV4(quad,
			/**/ right_half_ofs, 0,
			/**/ right_half_ofs + SCREEN_SIZE_X / 2, 0,
			/**/ right_half_ofs, 239,
			/**/ right_half_ofs + SCREEN_SIZE_X / 2, 239);

		DR_MOVE *last_frame =
			(DR_MOVE *)new_primitive(&ctx, 0, sizeof(DR_MOVE));
		RECT source_rect;
		source_rect.x = 0;
		source_rect.y = (ctx.active_buffer) * SCREEN_SIZE_Y;
		source_rect.w = SCREEN_SIZE_X;
		source_rect.h = SCREEN_SIZE_Y;
		SetDrawMove(last_frame, &source_rect, SCREEN_SIZE_X, 0);

		// Draw some text in front of the square (Z = 0, primitives with higher
		// Z indices are drawn first and thus drawn "behind" this).
		draw_text(&ctx, 8, 16, 0, "Hello, world 3!");

		// Draw a frames clock...
		char clock_buff[8];
		sprintf(clock_buff, /* sizeofarr(clock_buff), */ "c: %04d", frames);
		draw_text(&ctx, 12, 24, 0, clock_buff);

		char ctl_buff[24];
		sprintf(ctl_buff, /* sizeofarr(ctl_buff), */
			"btn: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
			(ctl.buffer[0].button & PAD_SELECT) ? '_' : 'S' /*'s'*/,
			(ctl.buffer[0].button & PAD_L3) ? '_' : 'L' /*'l'*/,
			(ctl.buffer[0].button & PAD_R3) ? '_' : 'R' /*'r'*/,
			(ctl.buffer[0].button & PAD_START) ? '_' : 'S' /*'s'*/,
			(ctl.buffer[0].button & PAD_UP) ? '_' : '^' /*'u'*/,
			(ctl.buffer[0].button & PAD_RIGHT) ? '_' : '>' /*'r'*/,
			(ctl.buffer[0].button & PAD_DOWN) ? '_' : 'v' /*'d'*/,
			(ctl.buffer[0].button & PAD_LEFT) ? '_' : '<' /*'l'*/,
			(ctl.buffer[0].button & PAD_L2) ? '_' : 'L' /*'l'*/,
			(ctl.buffer[0].button & PAD_R2) ? '_' : 'R' /*'r'*/,
			(ctl.buffer[0].button & PAD_L1) ? '_' : 'L' /*'l'*/,
			(ctl.buffer[0].button & PAD_R1) ? '_' : 'R' /*'r'*/,
			(ctl.buffer[0].button & PAD_TRIANGLE) ? '_' : 'T' /*'t'*/,
			(ctl.buffer[0].button & PAD_CIRCLE) ? '_' : 'O' /*'o'*/,
			(ctl.buffer[0].button & PAD_CROSS) ? '_' : 'X' /*'x'*/,
			(ctl.buffer[0].button & PAD_SQUARE) ? '_' : 'Q' /*'q'*/);
		draw_text(&ctx, 12, 220, 0, ctl_buff);

		flip_buffers(&ctx);

		frames++;
	}

	return 0;
}
