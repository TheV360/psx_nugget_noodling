#include "player.h"

#include <libgpu.h> // primitive structs and macros

void setup_player(e_player *e) {
	e->x = e->y = 0;
	e->vx = e->vy = 0;
}

void control_player(e_player *e, GamepadState *ctl) {
	GamepadButtonWatcher btn = ctl->button[0];

	if (btn.hold_time[BTN_LEFT]) e->vx = -FX_LIT(5, FX_GTE_P);
	if (btn.hold_time[BTN_RIGHT]) e->vx = FX_LIT(5, FX_GTE_P);

	if (btn.hold_time[BTN_CROSS]) e->vy = -FX_LIT(7, FX_GTE_P);
}

void update_player(e_player *e) {
	e->x += e->vx;
	e->y += e->vy;

	e->vy += FX_LIT(5, FX_GTE_P - 4);

	int       slow;
	const int slow_roll = 8;

	slow = (e->vx) / slow_roll;
	if (slow == 0)
		e->vx = 0;
	else
		e->vx -= slow;

	slow = (e->vy) / slow_roll;
	if (slow == 0)
		e->vy = 0;
	else
		e->vy -= slow;

	if (e->y > FX_LIT(200, FX_GTE_P)) {
		e->y = FX_LIT(200, FX_GTE_P);
		e->vy = 0;
	}
}

void draw_player(e_player *e, RenderContext *ctx) {
	POLY_G3 *tri = (POLY_G3 *)new_primitive(ctx, 1, sizeof(POLY_G3));

	setPolyG3(tri);
	setXY3(tri,
		/**/ FX_INT(e->x, FX_GTE_P), FX_INT(e->y, FX_GTE_P) - 16,
		/**/ FX_INT(e->x, FX_GTE_P) - 5, FX_INT(e->y, FX_GTE_P),
		/**/ FX_INT(e->x, FX_GTE_P) + 5, FX_INT(e->y, FX_GTE_P));

	setRGB0(tri, 255, 0, 0);
	setRGB1(tri, 0, 255, 0);
	setRGB2(tri, 0, 0, 255);
}
