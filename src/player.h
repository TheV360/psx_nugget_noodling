#include "types.h"

#include "fixed_point.h"

#include "system/gamepad.h"  // -> GamepadState
#include "system/graphics.h" // -> RenderContext

typedef struct {
	int x, y;
	int vx, vy;
} e_player;

void setup_player(e_player *e);
void control_player(e_player *e, GamepadState *ctl);
void update_player(e_player *e);
void draw_player(e_player *e, RenderContext *ctx);
