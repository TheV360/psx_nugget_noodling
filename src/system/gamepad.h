#pragma once

#include "../types.h"

typedef enum {
	BTN_SELECT = 0,
	BTN_L3,
	BTN_R3,
	BTN_START,
	BTN_UP,
	BTN_RIGHT,
	BTN_DOWN,
	BTN_LEFT,
	BTN_L2,
	BTN_R2,
	BTN_L1,
	BTN_R1,
	BTN_TRIANGLE,
	BTN_CIRCLE,
	BTN_CROSS,
	BTN_SQUARE,
	BTN_MAX
} GamepadButtons;

typedef struct {
	u8 status;

	u8 length : 4;
	u8 type : 4;

	u16 button;

	// remember deadzones exist!!
	struct {
		u8 x, y;
	} stick_right;
	struct {
		u8 x, y;
	} stick_left;
} GamepadBuffer;

typedef struct {
	struct {
		u16 *watch;
		u16  previous;
	} internal;
	// just to compartmentalize the counterintuitive inverted raw data.

	u8 hold_time[BTN_MAX];

	u16 pressed;
	u16 released;
} GamepadButtonWatcher;

typedef struct {
	union {
		byte          buffer_raw[2][34];
		GamepadBuffer buffer[2];
	};
	GamepadButtonWatcher button[2];
} GamepadState;

void setup_gamepad(GamepadState *);
void update_gamepad(GamepadState *);

void update_watcher(GamepadButtonWatcher *);
