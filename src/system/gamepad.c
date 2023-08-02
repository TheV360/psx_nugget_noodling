#include "gamepad.h"

#include <libapi.h>

void setup_gamepad(GamepadState *ctl) {
	const usize buff_size = sizeofarr(ctl->buffer[0]);
	InitPAD(ctl->buffer[0], buff_size, ctl->buffer[1], buff_size);

	// Make sure first-frame state is "invalid" rather than "ok".
	ctl->buffer[0][0] = ctl->buffer[0][1] = (char)0xFF;
	ctl->buffer[1][0] = ctl->buffer[1][1] = (char)0xFF;

	StartPAD();
	ChangeClearPAD(1); // disable the dang automatic VSYNC acknowledgement
}
