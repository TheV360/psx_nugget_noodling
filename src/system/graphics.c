#include "graphics.h"

#include <assert.h> // -> assert

void setup_context(RenderContext *ctx, int w, int h) {
	// Place the two framebuffers vertically in VRAM.
	SetDefDrawEnv(&(ctx->buffers[0].draw_env), 0, 0, w, h);
	SetDefDispEnv(&(ctx->buffers[0].disp_env), 0, 0, w, h);
	SetDefDrawEnv(&(ctx->buffers[1].draw_env), 0, h, w, h);
	SetDefDispEnv(&(ctx->buffers[1].disp_env), 0, h, w, h);

	// Initialize the first buffer and clear its ordering table
	// so that it can later be used for drawing.
	ctx->active_buffer = 0;
	RenderBuffer *curr_buffer = &(ctx->buffers[ctx->active_buffer]);
	ctx->next_packet = curr_buffer->buffer;
	ctx->packet_buffer_end = &(curr_buffer->buffer[BUFFER_LENGTH]);
	ClearOTagR(curr_buffer->ord_tbl, OT_LAYERS);

	// Turn on the video output.
	SetDispMask(1);
}

void set_clear_colors(RenderContext *ctx, u8 r, u8 g, u8 b) {
	// Set the default background color and enable auto-clearing.
	setRGB0(&(ctx->buffers[0].draw_env), r, g, b);
	setRGB0(&(ctx->buffers[1].draw_env), r, g, b);
	ctx->buffers[0].draw_env.isbg = true;
	ctx->buffers[1].draw_env.isbg = true;
}

void unset_clear_colors(RenderContext *ctx) {
	ctx->buffers[0].draw_env.isbg = false;
	ctx->buffers[1].draw_env.isbg = false;
}

void flip_buffers(RenderContext *ctx) {
	// Wait for the GPU to finish drawing the frame,
	// then wait for the TV to finish presenting the previous frame.
	DrawSync(0);
	VSync(0);

	RenderBuffer *draw_buffer = &(ctx->buffers[ctx->active_buffer]);
	RenderBuffer *disp_buffer = &(ctx->buffers[ctx->active_buffer ^ 1]);

	// Display the framebuffer the GPU has just finished drawing and start
	// rendering the display list that was filled up in the main loop.
	PutDispEnv(&(disp_buffer->disp_env));
	DrawOTagEnv(
		&(draw_buffer->ord_tbl[OT_LAYERS - 1]), &(draw_buffer->draw_env));

	// Switch over to the next buffer, clear it
	// and reset the packet allocation pointer.
	ctx->active_buffer ^= 1;
	ctx->next_packet = disp_buffer->buffer;
	ctx->packet_buffer_end = &(disp_buffer->buffer[BUFFER_LENGTH]);
	ClearOTagR(disp_buffer->ord_tbl, OT_LAYERS);
}

void *new_primitive(RenderContext *ctx, int z, size_t size) {
	// Place the primitive after all previously allocated primitives, then
	// insert it into the OT and bump the allocation pointer.
	RenderBuffer *buffer = &(ctx->buffers[ctx->active_buffer]);
	void         *prim = ctx->next_packet;

	addPrim(&(buffer->ord_tbl[z]), prim);
	ctx->next_packet += size;

	// Make sure we haven't yet run out of space for future primitives.
	assert(ctx->next_packet <= ctx->packet_buffer_end);

	return (void *)prim;
}
