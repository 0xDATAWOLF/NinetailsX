# Builds (and configures CMake if necessary) the project.

if (!(Test-Path -Path './build'))
{
	./config_cmake.ps1
}

cmake --build ./build