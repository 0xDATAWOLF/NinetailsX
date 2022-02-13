/**
 * 
 * The big-bad todo list & bug-fix list!
 * 
 * TESTING:			Memory functions (memset/memcopy)
 * 			I actually haven't tested these functions to determine if they're working the way they're
 * 			designed. I'm assuming that they work because I implemented them in a very similar way in
 * 			the past that I know to work, but you never know unless I do proper debugging. For now,
 * 			it doesn't seem to cause any segment faults (since the bitmap buffer is used on the tail
 * 			end of the heap allocation, any over-runs would cause a seg-fault).
 * 
 * BUGFIX:			Frame timing
 * 			There is most certainly a bug with the frame timing and sleep protocol which will need to
 * 			be ironed out and refactored soon enough. While the application itself seems to work as
 * 			intended, residual framesync issues persist until timing falls back in line. Whether or
 * 			not that's the actual case or not is yet to be determined.
 */

#include "main.h"
#include "display.h"
#include <nxcore/string.h>
#include <stdio.h>

/**
 * This will load the engine library code and assign it to the struct which carries the
 * pointers to the necessary functions.
 * 
 * LoadLibraryA:
 * 			https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya
 * 
 * GetProcAddress:
 * 			Pulling out the procedures loaded in by the DLL.
 * 			https://docs.microsoft.com/en-us/windows/desktop/api/libloaderapi/nf-libloaderapi-getprocaddress
 * 
 * FreeLibrary:
 * 			If we want to support hot-loading.
 * 			https://docs.microsoft.com/en-us/windows/desktop/api/libloaderapi/nf-libloaderapi-freelibrary
 * 
 */
internal i32
InitializeNinetailsXEngine(char* dynamicLibraryFilePath, engine_library* EngineLibrary)
{

	HMODULE EngineModule = LoadLibraryA(dynamicLibraryFilePath);

#ifdef NINETAILSX_DEBUG
	assert(EngineModule != NULL);
#else
	return 0;
#endif

	/**
	 * When we pull from the engine library, we are expecting that the procedure addresses are always going
	 * to be valid. They might not be--if they're null, or not what we're we expected them to be *outside*
	 * of testing--that's a user-error problem. Someone touched the supplied DLL when they shouldn't be touching
	 * the DLL. What happens afterwards is entirely undefined.
	 * 
	 * TODO:
	 * 			Maybe we play nice and just yeet into a fail-fast state and exit. Maybe.
	 */
	EngineLibrary->EngineRuntime = (fnptr_engine_runtime*)GetProcAddress(EngineModule, "EngineRuntime");
	EngineLibrary->EngineInit = (fnptr_engine_init*)GetProcAddress(EngineModule, "EngineInit");
	EngineLibrary->EngineReinit = (fnptr_engine_reinit*)GetProcAddress(EngineModule, "EngineReinit");

#ifdef NINETAILSX_DEBUG
	assert(EngineLibrary->EngineRuntime != NULL);
	assert(EngineLibrary->EngineInit != NULL);
	assert(EngineLibrary->EngineReinit != NULL);
#endif

	return 1;
}

/**
 * RenderSoftwareBitmap
 * 			Renders a given bitmap image to the screen. This is effectively a software-only rendering
 * 			method. Therefore, this shouldn't be used for anything that may be hardware intensive and
 * 			primarily used for basic 2D drawing.
 * 
 * StretchDIBits
 * 				We will use this as our primary mode outputting to the window.
 * 				https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-stretchdibits
 */
internal void
RenderSoftwareBitmap(app_state* ApplicationState, void* BitmapData, i32 BitmapWidth, i32 BitmapHeight)
{

	BITMAPINFO BitmapInfo = {0};
	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	BitmapInfo.bmiHeader.biSizeImage = BitmapWidth*BitmapHeight*sizeof(u32);

	i32 Status = StretchDIBits(ApplicationState->WindowDeviceContext, 0, 0, BitmapWidth, BitmapHeight, 0, 0,
		BitmapWidth, BitmapHeight, BitmapData, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);

	if (Status)
	{
		// do something
		i32 HelloWorld = 0;
	}

}

LRESULT CALLBACK
WindowProcedure(HWND WindowHandle, u32 Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_CREATE:
		{
			
		} break;

		case WM_CLOSE:
		case WM_DESTROY:
		{
			/**
			 * NOTE:
			 * 			This is where we *might* do some clean up if it was entirely necessary to do so.
			 * 			For the most part, we may need to quickly save up any data before exiting, but
			 * 			freeing memory here is entirely optional because the OS will gobble that back up.
			 */
			ApplicationState->isRunnning = false;
		} break;

		case WM_SETCURSOR:
		{
			/**
			 * This event is triggered when the mouse enters the windows.
			 * 
			 * NOTE:
			 * 			If we use a custom cursor, then we will need to account for that here.
			 * 			Otherwise, we will default to the pointer.
			 */
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		} break;

		case WM_PAINT:
		{

			/**
			 * This event is generated when the system or another application requests that the window
			 * needs to redraw itself. This is most likely triggered when the window is resized or moved.
			 * 
			 * NOTE:
			 * 			The action of this event uses painting to fill in the gaps, but hardware acceleration
			 * 			will eventually take over this for us. We will probably want to change the behavior of
			 * 			this event depending on the graphics API we chose to use (Vulkan, OpenGL, DX12).
			 * 
			 * WM_PAINT:
			 * 			https://docs.microsoft.com/en-us/windows/win32/gdi/wm-paint
			 * 
			 * BeginPaint:
			 * 			https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-beginpaint
			 * 			
			 */

			BOOL PaintRequired = GetUpdateRect(WindowHandle, NULL, false);
			if (!PaintRequired) break; // Break out from the next steps.

			PAINTSTRUCT PaintRegion = {0};
			HDC PaintDeviceContext = BeginPaint(WindowHandle, &PaintRegion);

			FillRect(PaintDeviceContext, &PaintRegion.rcPaint, GetSysColorBrush(COLOR_WINDOW));

			/**
			 * TODO:
			 * 			What are we supposed to do in this situation? If the window is resized, then
			 * 			it will *unsize* itself because the engine enforces a specific size every
			 * 			frame, right?
			 */
			//RenderSoftwareBitmap();

			EndPaint(WindowHandle, &PaintRegion);

		} break;

		default:
		{
			return(DefWindowProcA(WindowHandle, Message, wParam, lParam));	
		} break;
	}

	return(DefWindowProcA(WindowHandle, Message, wParam, lParam));
}

/**
 * Retrieves the current performance counter from QueryPerformanceCounter and returns
 * it as a 64-bit unsigned integer.
 */
inline u64
GetCurrentPerformanceStamp()
{

	LARGE_INTEGER Timestamp;
	QueryPerformanceCounter(&Timestamp);
	return Timestamp.QuadPart;

}

/**
 * Retrives the current time in milliseconds using a previous QPC timestamp and
 * returns it as a float.
 */
inline r32
GetPerformanceTimeDifferenceMilliseconds(u64 frequency, u64 previous)
{
	u64 now = GetCurrentPerformanceStamp();
	u64 difference = now - previous;
	u64 differenceMilliseconds = difference * 1000; // Conversion to milliseconds.
	r32 Time = (r32)differenceMilliseconds / (r32)frequency;
	return Time;
}

/**
 * Fetches the state of keyboard using a key code and determines the state of input.
 */
internal void
setInputButtonState(u8 keyCode, button* previousInputButton, button* currentInputButton)
{
	u32 keyState = (GetKeyState(keyCode) & (1 << 15));
	if (previousInputButton->down && keyState == 0)
	{
		currentInputButton->released = true;
		currentInputButton->down = false;
	}
	else if (keyState != 0)
	{
		currentInputButton->down = true;
		currentInputButton->released = false;
	}
}

/**
 * Fetches the size of a file from a relative path. This function looks at app_state for
 * BasePath, therefore it must be properly set in order for it to construct a valid
 * path. Additionally, this function will work only for files under 4GB.
 * 
 * TODO:
 * 			We should assume a return value of 0 is assumed to be a failure.
 */
internal u32
FetchResourceSize(char* RelativePath)
{

	char _absolute_path[MAX_PATH];

	ConcatenateStrings_s(ApplicationState->BasePath, MAX_PATH, RelativePath,
		StringSize(RelativePath), _absolute_path, MAX_PATH);

	HANDLE _resource_handle = CreateFileA(RelativePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	assert(_resource_handle != INVALID_HANDLE_VALUE);

	LARGE_INTEGER _file_size = {0};
	BOOL FileSizeStatus = GetFileSizeEx(_resource_handle, &_file_size);

	/**
	 * We don't necessary expect the files to be over the 4GB limit, but if they are we certainly
	 * should just assert in case they are. Generally speaking, we also expect that read succeeded.
	 */
#ifdef NINETAILSX_DEBUG
	assert(FileSizeStatus);
	assert(_file_size.QuadPart < 0xFFFFFFFF);
#endif

	CloseHandle(_resource_handle);

	return _file_size.LowPart;

}

/**
 * Fetches a file and stores the contents of the file into the buffer.
 * This function looks at app_state for BasePath, therefore it must be
 * properly set in order for it to construct a valid path. Additionally,
 * this function will work only for files under 4GB.
 * 
 * TODO:
 * 			We might want to report back to the engine that status of the
 * 			operation. For now, we will just assert if the file hasn't
 * 			been found or unable to be loaded.
 */
internal u32
FetchResourceFile(char* RelativePath, void* Buffer, u32 BufferSize)
{

	char _absolute_path[MAX_PATH];

	ConcatenateStrings_s(ApplicationState->BasePath, MAX_PATH, RelativePath,
		StringSize(RelativePath), _absolute_path, MAX_PATH);

	HANDLE _resource_handle = CreateFileA(RelativePath, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	assert(_resource_handle != INVALID_HANDLE_VALUE);

	DWORD BytesRead = 0;
	BOOL ReadStatus = ReadFile(_resource_handle, Buffer, BufferSize, &BytesRead, 0);

	assert(ReadStatus);
	assert(BytesRead == BufferSize);

	return 1; // We assume that the read succeeded.

}


/**
 * Defines the entry point for a win32 application.
 */
i32 WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PWSTR Commandline, int CommandShow)
{

	/**
	 * We must guarantee the existence of ApplicationState on startup. Here, we will initialize it
	 * and set the variable as declared in main.h.
	 */
	app_state _appState;
	ApplicationState = &_appState;
	renderer* Renderer = &ApplicationState->Renderer;

	/**
	 * Create & fill out the WNDCLASS struct.
	 * 
	 * NOTE:	
	 * 			Do we want CS_OWNDC? Check with OpenGL and see if this is something we
	 * 			are interested in. Otherwise, we should remove this.
	 * 
	 * Style:
	 * 			For reference if we want to modify the window styles.
	 * 			https://docs.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
	 * 
	 * Cursors:
	 * 			We will want to change the cursors (and eventually remove it all together in favor
	 * 			of custom cursors created through our rendering engine).
	 * 			https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-setcursor
	 * 			https://docs.microsoft.com/en-us/windows/desktop/menurc/cursors
	 * 
	 * Icons:
	 * 			We will want to load our own icons in the future.
	 * 			https://docs.microsoft.com/en-us/windows/desktop/menurc/icons
	 */
	DWORD WindowStyle = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

	WNDCLASSA WindowClass = {0};
	WindowClass.lpszClassName = "NinetailsX";
	WindowClass.hInstance = hInstance;
	WindowClass.lpfnWndProc = &WindowProcedure;
	WindowClass.style = WindowStyle;
	WindowClass.hCursor = NULL;
	WindowClass.lpszMenuName = NULL;

	RegisterClassA(&WindowClass);

	/**
	 * The system will determine the size of the window using defaults, however we want to set the
	 * size of the to a particular size. The CreateWindowExA size sets the window's total size (this
	 * includes the window frame, menu, etc.), not the actual "client area" which we modify.
	 * 
	 * We are setting that here and then creating the window using that information.
	 * 
	 * Then we are relaying this message to the renderer.
	 * 
	 * AdjustWindowRect:
	 * 			We can get the correct window size based on our requested dimensions by calling this function.
	 * 			https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-adjustwindowrect
	 * 
	 * 			It's important to note that this is based on the dwStyle parameters; the size is dependent on
	 * 			this data being correct.
	 * 			https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	 * 
	 * 			Since we use "WS_OVERLAPPEDWINDOW" as our style, our window uses a frame (with min/max, close controls)
	 * 			and therefore we need to provide WS_CAPTION which includes WS_BORDER. This will gives us the appropriate
	 * 			window style.
	 */

#define INIT_WINDOW_WIDTH 1280
#define INIT_WINDOW_HEIGHT 720
	v2i InitWindowDimensions = CalculateWindowSizeFromClientSize({INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT});

	HWND WindowHandle = CreateWindowExA(0, "NinetailsX", "NinetailsX Engine",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, InitWindowDimensions.width, InitWindowDimensions.height, NULL,
		NULL, hInstance, NULL);

	Renderer->WindowDimensions = InitWindowDimensions;

	/** 
	 * NOTE:
	 * 			We are owning the DC here, so we won't need to release it *unless* we remove CS_OWNDC!
	 * 			Therefore, if this was to ever change, we will need to release our device context after
	 * 			we use it.
	 */
	ApplicationState->WindowDeviceContext = GetDC(WindowHandle);

#ifdef NINETAILSX_DEBUG
	// Ensuring that we actually got the window handle.
	assert(WindowHandle != NULL);

	// Ensuring the window size we requested is the actual window size that we have.
	RECT ActualClientSize = {0};
	GetClientRect(WindowHandle, &ActualClientSize);
	i32 ActualWindowWidth = ActualClientSize.right-ActualClientSize.left;
	i32 ActualWindowHeight = ActualClientSize.bottom-ActualClientSize.top;
	assert(ActualWindowWidth == INIT_WINDOW_WIDTH);
	assert(ActualWindowHeight == INIT_WINDOW_HEIGHT);
#endif

	/**
	 * Setting up performance timing for the application runtime.
	 * 
	 * TODO:
	 * 			We need to set up a way of maintaining some form of frame-timing and performance monitoring.
	 * 			This will be accomplished using Windows' QPC.
	 * 
	 * QueryPerformanceCounter
	 * 			https://docs.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps
	 */
	LARGE_INTEGER PerformanceFrequency;
	QueryPerformanceFrequency(&PerformanceFrequency);
	ApplicationState->PerformanceFrequency = PerformanceFrequency.QuadPart;

	/**
	 * Allocate the heap necessary for application runtime. These are set up in the ApplicationStates'
	 * memory_layout member such that we can feed this to the engine DLL.
	 * 
	 * NOTE:
	 * 			We will need to *dynamically* consider what the user's system is capable of, but for
	 * 			now we will delegate a fixed size at compile time during development.
	 * 
	 * VirtualAlloc:
	 * 			https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc
	 */
#define VIRTUAL_ALLOCATION_SIZE Megabytes(512)
#ifdef NINETAILSX_DEBUG
	void* HeapMemory = VirtualAlloc((void*)Terabytes(2), VIRTUAL_ALLOCATION_SIZE, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
#else
	void* HeapMemory = VirtualAlloc((void*)0x00, VIRTUAL_ALLOCATION_SIZE, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
#endif
	ApplicationState->MemoryLayout.Base = HeapMemory;
	ApplicationState->MemoryLayout.Size = VIRTUAL_ALLOCATION_SIZE;
	memory_layout* GameMemoryLayout = &ApplicationState->MemoryLayout;

	/**
	 * In order for us to do any file IO operations, we need to create a base path for which we can
	 * navigate "relatively". We can do this by fetching the module name of the executable. From there,
	 * we can pull out the root directory by finding the "last slash" and then stripping out the executable
	 * name out of it. Then we store that in app_state for future use for path operations.
	 * 
	 * GetModuleFileNameA:
	 * 			This will give us the path of the executable of the module.
	 * 			https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
	 */

	// Retrieve module path.
	char executablePath[MAX_PATH];
	char modulePath[MAX_PATH];
	char* moduleName = "NinetailsXEngine.dll";
	GetModuleFileNameA(NULL, executablePath, MAX_PATH);

	// Find location of last slash.
	char* lastSlash = executablePath;
	for (i32 offset = 0; executablePath[offset]; ++offset)
	{
		if (executablePath[offset] == '\\')
		{
			lastSlash = &executablePath[offset+1];
		}
	}

	// Copy everything up to last slash then null-terminate the string to get root dir.
	u32 BasePathCount = (u32)(lastSlash - executablePath);
	nx_memcopy(ApplicationState->BasePath, executablePath, BasePathCount);
	*(ApplicationState->BasePath + BasePathCount) = '\0';

	// We need to load the engine module into memory, so we will do that here by concat'ing the 
	// relative directory string to the root path.
	ConcatenateStrings_s(ApplicationState->BasePath, MAX_PATH, "NinetailsXEngine.dll", (u32)sizeof("NinetailsXEngine.dll"), modulePath, MAX_PATH);
	InitializeNinetailsXEngine(modulePath, &ApplicationState->EngineLibrary);

	/**
	 * We need to initialize the resource handler functions for the engine to call.
	 */

	ApplicationState->ResourceHandlerInterface.FetchResourceFile = &FetchResourceFile;
	ApplicationState->ResourceHandlerInterface.FetchResourceSize = &FetchResourceSize;


	/**
	 * We need to set up the input swap buffer. We need to manage the current input and the previous
	 * so we can determine whether or not a button was released. We can only determine this information
	 * (using the method I am currently using) by using a swap buffer.
	 */
	input* currentInput = &ApplicationState->InputSwapBuffer[0];
	input* previousInput = &ApplicationState->InputSwapBuffer[1];
	*currentInput = {0};
	*previousInput = {0};

	/**
	 * We need to ensure that the Windows scheduler is fine-grained enough for us to hit our software
	 * v-sync time consistently. To do this, we will use timeBeginPeriod(1). Then we need to establish
	 * what our vsync frame target is. This is relatively easy to calculate.
	 * 
	 * NOTE:
	 * 			The frameTarget will need to be adjusted should we begin to miss our frame targets.
	 * 
	 * NOTE:
	 * 			Additionally, since the scheduler may not wake up on the exact moment we want, we an
	 * 			"offset" of sorts such that we can "busy-spin" the remainder of the frame for the highest
	 * 			accuracy "flip-time".
	 */
	MMRESULT timePeriodResult = timeBeginPeriod(1);
	r32 frameTarget = (r32)1000 / 60; // 1000ms = 1s diveded by how many frames per second = how many ms/frame
	r32 sleepOffset = 1.0;

#ifdef NINETAILSX_DEBUG
	// We want to make sure we get our requested time period in debug.
	assert(timePeriodResult == TIMERR_NOERROR);
#endif

	/**
	 * We need to perform engine initialization before we show the window. This will handle any window
	 * updates if there are any. We also need to handle any window sizing requests here as well. Once
	 * the init finishes up, we can begin showing the window and initiated the engine runtime.
	 */
	engine_library& EngineLib = ApplicationState->EngineLibrary;
	EngineLib.EngineInit(GameMemoryLayout, Renderer, &ApplicationState->ResourceHandlerInterface);

	if (GetWindowClientSize(WindowHandle) != Renderer->WindowDimensions)
		SetWindowClientSize(WindowHandle, Renderer->WindowDimensions);


	/**
	 * Begin the application runtime loop given that we have sucessfully established and loaded all
	 * the necessary facilities to reach this point. We will show the window at this point.
	 */
	ShowWindow(WindowHandle, CommandShow);
	ApplicationState->isRunnning = true;
	u64 InitialFrameStamp = GetCurrentPerformanceStamp();
	while (ApplicationState->isRunnning)
	{

		/**
		 * NOTE:
		 * 			We are handling the window messages, but the state changes happen within
		 * 			the WindowProcedure function. If WM_CLOSE or WM_DESTROY are called, then
		 * 			it will force the application to close on the next loop iteration.
		 * 			This may not be desirable since that causes a single frame of computation
		 * 			to go through. Instead, we should consider examining the messages here and
		 * 			promptly breaking out of the loop when it is necessary.
		 */
		MSG Message = {0};
		while (PeekMessageA(&Message, WindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessageA(&Message);
		}

		/**
		 * We have to process input for the engine, so we will do that here. This will also carry over
		 * the frameStep/deltaTime over to the client, so we need to make sure that we're sending correct
		 * data.
		 * 
		 * Additionally, we will capture the keyboard state as it were since the last message capture.
		 * This is fine because we pump the message loop at the head of every frame.
		 * 
		 * GetKeyState:
		 * 			https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeystate
		 */
		ApplicationState->InputHandle.frameStep = frameTarget; // Consistent frame steps need target, not actual!
		ApplicationState->InputHandle.frame_input = currentInput;
		*currentInput = {0}; // Reset

		setInputButtonState('Z', &previousInput->aButton, &currentInput->aButton);
		setInputButtonState('X', &previousInput->bButton, &currentInput->bButton);
		setInputButtonState(VK_RETURN, &previousInput->startButton, &currentInput->startButton);
		setInputButtonState(VK_RSHIFT, &previousInput->selectButton, &currentInput->selectButton);
		setInputButtonState(VK_RIGHT, &previousInput->rightButton, &currentInput->rightButton);
		setInputButtonState(VK_LEFT, &previousInput->leftButton, &currentInput->leftButton);
		setInputButtonState(VK_UP, &previousInput->upButton, &currentInput->upButton);
		setInputButtonState(VK_DOWN, &previousInput->downButton, &currentInput->downButton);

		/**
		 * We are executing the engine runtime here.
		 * 
		 * NOTE:
		 * 			The engine runtime contains a number of side-effects we will need to consider.
		 * 			The first side effect is that the engine may request a new window size. We will handle
		 * 			this accordingly.
		 * 
		 * 			The return value of EngineRuntime() determines if the application should close based
		 * 			on the request of the engine client or user. We need to respect this decision by
		 * 			exitting gracefully. For now, we are assuming that all non-zero values request a close.
		 * 			
		 * TODO:
		 * 			Define an enumeration outlining various reasons for closing, such as standard exits,
		 * 			error exits, re-init exits, etc.
		 */
		b32 EngineStatus = EngineLib.EngineRuntime(GameMemoryLayout, Renderer, &ApplicationState->InputHandle); 

		// If the engine status returns non-zero status, it means we should close.
		if (EngineStatus != NULL)
		{
			// TODO: Implement me! :)
		}

		// The engine may request the window be resized the accomodate the size of the render area.
		if (GetWindowClientSize(WindowHandle) != Renderer->WindowDimensions)
			SetWindowClientSize(WindowHandle, Renderer->WindowDimensions);

		/**
		 * Now that the frame is completed, we can swap the input buffers for the next frame.
		 */
		input** placeholder = &currentInput;
		currentInput = previousInput;
		previousInput = *placeholder;
		
		/**
		 * We need to create some v-sync for fixed-physics timescale or something close to it.
		 * 
		 * NOTE:
		 * 			There are some limitations to using sleep as a form of software v-sync. Since
		 * 			the Windows scheduler does not guarantee that it will switch to our process on time,
		 * 			we may go below or above our requested sleep time.
		 * 
		 * 			We *could* busy spin for the highest-accuracy swap interval, as it will generally
		 * 			be granular enough for us to hit our vsync timing with great precision. The problem
		 * 			is that it will busy-spin, burning up the CPU for no other reason but to keep accuracy.
		 * 			Instead, we can under-estimate our sleep time, guaranteeing that we are short of flip,
		 * 			busy spin the difference, and keep close to our frame target.
		 */
		r32 frameTime = GetPerformanceTimeDifferenceMilliseconds(ApplicationState->PerformanceFrequency,
			InitialFrameStamp);
		r32 initFrameTime = frameTime; // frameTime will change during the sleep loop, let's preserve it here.

		r32 sleepTime = frameTarget - frameTime;
		r32 initSleepTime = (r32)((u32)(sleepTime - sleepOffset));
		if (sleepTime > 0.0f)
		{
			Sleep((u32)(sleepTime-sleepOffset)); // Sleep wants flat ms, so we just truncate the sleep time.
			frameTime = GetPerformanceTimeDifferenceMilliseconds(ApplicationState->PerformanceFrequency, InitialFrameStamp);
			sleepTime = frameTarget - frameTime;
		}

		r32 busySpinTime = sleepTime; // We want to preserve how long we spend busy-spinning.
		if (busySpinTime > 0.0f)
		{
			while (frameTime < frameTarget)
			{
				frameTime = GetPerformanceTimeDifferenceMilliseconds(ApplicationState->PerformanceFrequency, InitialFrameStamp);
				sleepTime = frameTarget - frameTime;
			}
		}

#ifdef NINETAILSX_DEBUG
		/**
		 * This will print out the Visual Studio's debug console the frame timings. We can use this to
		 * do simple performance monitoring.
		 */
		char frameDebugString[256];
		sprintf_s(frameDebugString, 256, "Frame Timing :: Target %.2fms | Actual %.2fms | Sleeptime %.2fs | Spintime %.2fms | Postsleep %.2fms \n",
			frameTarget, initFrameTime, initSleepTime, busySpinTime, frameTime);
		OutputDebugStringA(frameDebugString);
#endif

		/**
		 * We are rendering the bitmap to the screen using a "software" implementation--that is, we're
		 * just drawing straight to the Window using Window's bitmap drawing method.
		 */
		RenderSoftwareBitmap(ApplicationState, Renderer->Image, Renderer->WindowDimensions.width, Renderer->WindowDimensions.height);

		/**
		 * We will now reset everything for the next frame.
		 */
		InitialFrameStamp = GetCurrentPerformanceStamp(); // Sets the timing of the next frame.

	}

	return(0);
}