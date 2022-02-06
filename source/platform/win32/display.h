#ifndef NINETAILSX_DISPLAY_H
#define NINETAILSX_DISPLAY_H
#include <nxcore/helpers.h>
#include <nxcore/math.h>
#include <windows.h>

/**
 * Retrieves the current size of the window's client area. The client area does not include the
 * window frame and menu controls.
 */
internal v2i
GetWindowClientSize(HWND WindowHandle)
{
	RECT CurrentWindowRect = {0};
	GetClientRect(WindowHandle, &CurrentWindowRect);
	i32 CurrentWindowWidth = CurrentWindowRect.right - CurrentWindowRect.left;
	i32 CurrentWindowHeight = CurrentWindowRect.bottom - CurrentWindowRect.top;
	return { CurrentWindowWidth, CurrentWindowHeight };
}

/**
 * Calculates the window size from a desired size. Since the client area isn't necessary the
 * actual size of a window, this function calculates the window size which accomodates the desired
 * client area size.
 * 
 * The default window styling option creates a frame with standard window controls and no menu.
 */
inline v2i
CalculateWindowSize(v2i WindowSize, DWORD WindowStyling = WS_CAPTION)
{
	RECT NewWindowRect = {0, 0, WindowSize.width, WindowSize.height};
	AdjustWindowRect(&NewWindowRect, WindowStyling, FALSE);
	i32 NewWindowWidth = NewWindowRect.right - NewWindowRect.left;
	i32 NewWindowHeight = NewWindowRect.bottom - NewWindowRect.top;
	return { NewWindowWidth, NewWindowHeight };
}

/**
 * Resizes the window's size to the desired size. When a window is resized with this function,
 * it recalculates the client size provided by the WindowSize and factors in the frame of the window
 * before resizing. This is because while we might want a particular resolution, window resize requests
 * impact the entire window, including the frame.
 */
internal void
SetWindowClientSize(HWND WindowHandle, v2i WindowSize)
{
	v2i TrueWindowSize = CalculateWindowSize(WindowSize);
	SetWindowPos(WindowHandle, NULL, NULL, NULL,
		TrueWindowSize.width, TrueWindowSize.height, SWP_NOMOVE|SWP_NOZORDER);
}

#endif
