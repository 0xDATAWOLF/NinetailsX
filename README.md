# NinetailsX Game Engine

The NinetailsX is a hobbyist game engine built from the ground up using C/C++. Everything written (with some exceptions) from scratch,
only utilizing libraries when it is absolutely necessary.

### Development Requirements

Windows/Linux, CMake, and a compiler which can compile C++17 is necessary. While this project is designed to work outside IDEs, you will need Visual Studio and GDB
for debugging. Any assets used for testing are *not* included. Be sure to enable `NINETAILSX_DEBUG` macro in the base `CMakeLists.txt` in order to ensure proper
assertions for more information on what the project is expecting to exist. In the event this changes, they will be bundled with the repository.

### Documentation

If I documented everything I wrote, then I would not have the time to write anything. It's best to look at the source code for documentation as I do a pretty good
job marking out what it is that I am doing.

### License

You may **not** use this project for commercial projects without first contacting the copyright holder for permission. (Not that you should use this, there are
better options out there!) If you care to contribute to the project, please do so by submitting pull requests and I will review them. Private use is fine so long
as that you make the source code easily and readily available (along with this README or a link to the repository is fine).