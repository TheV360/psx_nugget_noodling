#include "gamepad.h"

#include <libapi.h>

void setup_gamepad(GamepadState *ctl) {
	const usize buff_size = sizeofarr(ctl->buffer_raw[0]);
	InitPAD(ctl->buffer_raw[0], buff_size, ctl->buffer_raw[1], buff_size);

	for (usize i = 0; i < sizeofarr(ctl->buffer); i++) {
		// Make sure first-frame state is "nothing" rather than "invalid".
		ctl->button[i].internal.previous = ctl->buffer[i].button = (u16)0xFFFF;

		// Set up watches
		ctl->button[i].internal.watch = &(ctl->buffer[i].button);

		// Zero out `hold_time`s
		for (usize j = 0; j < sizeofarr(ctl->button[i].hold_time); j++)
			ctl->button[i].hold_time[j] = 0;
	}

	StartPAD();
	ChangeClearPAD(1); // disable the dang automatic VSYNC acknowledgement
}

void update_gamepad(GamepadState *ctl) {
	// Each frame, the PSX gives new controller data. We don't *need*
	// to do any more. This is pretty much just quality-of-life stuff.

	update_watcher(&(ctl->button[0]));
	update_watcher(&(ctl->button[1]));
}

void update_watcher(GamepadButtonWatcher *button) {
	u16 current = !*(button->internal.watch);
	u16 previous = button->internal.previous;
	button->internal.previous = current;

	button->pressed = current ^ !previous;
	button->released = previous ^ !current;

	// TODO: implement `hold_time` lol
}
