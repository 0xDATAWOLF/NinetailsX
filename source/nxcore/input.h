#ifndef NINETAILSX_INPUT_H
#define NINETAILSX_INPUT_H
#include <nxcore/primitives.h>

/**
 * Represents an analog interface (mouse, joystick, etc).
 */
typedef struct analog
{
	/**
	 * TODO:
	 * 			We will need to implement controller input at some point.
	 */
	b32 _filler;
} analog;

/**
 * Represents a digital interface (key presses, mouse-button presses, controller-button presses, etc).
 */
typedef struct button
{
	b32 down;
	b32 released;
} button;

/**
 * The current frame's input state.
 */
typedef struct input
{
	button aButton;
	button bButton;
	button startButton;
	button selectButton;
	button leftButton;
	button rightButton;
	button upButton;
	button downButton;
} input;

typedef struct action_interface
{
	input* frame_input;
	r32 frameStep; // The v-sync frame timing for physics calculations.
} action_interface;

#endif