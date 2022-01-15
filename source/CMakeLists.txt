add_subdirectory(nxcore)

if (WIN32)
	message("System detected, WIN32, creating platform executable for Windows.")
	add_subdirectory(platform/win32)
endif ()

if (UNIX AND NOT APPLE)
	message("System detected, UNIX, creating platform executable for Linux.")
	add_subdirectory(platform/linux)
endif ()

target_include_directories(nxcore INTERFACE ./)