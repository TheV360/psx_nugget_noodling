#pragma once

#include "../types.h"

typedef struct {
	char buffer[2][34];
	bool connected;
} GamepadState;

void setup_gamepad(GamepadState *);
void update_gamepad(GamepadState *);
