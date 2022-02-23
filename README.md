# NinetailsX Graphics Engine

The NinetailsX is a hobbyist graphics engine built from the ground up using C/C++. The current status
of the project focuses primary on 2D software rendering (no assistance from the GPU). Additionally,
while I do intend to extend support across multiple platforms, this project is primarily built on and
for Windows.

### Development Requirements

Windows, CMake, and a compiler which can compile C++17 is necessary for a near headache-less experience.

### Notes on Documentation

Due to the early nature of the project, documentation is rather limited. The ever-evolving nature means
that all existing APIs are subject to change frequently, therefore I can not provide much documentation
outside the source code itself. I highly encourage exploring the source code, as I do make an effort to
document what I am doing, what it is for, and how it works.

### License Requirements

You may **not** use this project for commercial purposes. You may use this for personal projects or,
if you plan to use this for commercial purposes, please contact me before moving forward.

<br>

# Documentation

The documentation below are a rough overview of what this engine is about, the current design philosophies,
and general roadmap and function of the API. As mentioned above, specific documentation is limited to the
source code. Please reference the source code for more information.

### 1. Platform and Engine Layers

The standard structure of this project involves a separation of "engine" and "platform", that is, all
code within the engine is independent from the platform it runs on. The platform code, such as Win32
(Windows), is responsible for loading the engine DLL, providing the required resources the engine needs
to run, and responding to modifications the engine may request. The purpose of this separation is simple:
write the engine (and game code) once, apply it to as many platforms as needed, when needed.

The platform entry point is: `/source/platform/*platform_name*/main.cpp`

The engine entry point is: `/source/nxcore/engine/engine.cpp`

### 2. Software Renderering

The current supported method of rendering with this engine is software rendering. Software rendering
uses the CPU and the builtin window framework of the platform it runs on to draw to the screen. In the
case of the Win32 API, this is done through the Windows GDI framework.

You can find all software rendering related functions and helpers in:
`/source/nxcore/renderer/software.h`

Since software rendering is the only method for rendering currently, the platform is expecting a
bitmap buffer of exactly the resolution it is set to.

### 3. Memory Management

Memory management is handled with custom allocators... really dumb allocators. The current method
for allocating and "deallocating" is done through monotonic allocators. The engine is currently set
up with a derivation of the monotonic allocator called a bottom-top monotonic allocator. When the
platform reserves a region of memory, this is given to the engine which formats it using this bottom-top
monotonic allocator. The bottom portion contains lifetime allocations (they aren't normally dealloc'd)
and the top portion contains temporary allocations.

The bottom grows up, the top grows down, shares the same space, and doesn't really care which end is
bigger/smaller. Allocations will fail if either end takes up too much space. This is fine because the
application readily deallocates the top portion as needed when needed. When space runs out, it means
either two things: a memory leak, or improper design.

You can find the definitions of the memory management at:
`source/nxcore/memory.h`

### 4. Primitive Types

The engine and platform have access to a set of pre-defined primitive types which correspond to the
size they represent. This relies on the `stdint.h` header since it guarantees that the sizes they
represent are the sizes they are defined to be.

<table>
	<tr>
		<th>Type</th>
		<th>Size</th>
		<th>Usage</th>
	</tr>
	<tr>
		<td>u8</td>
		<td>8-bit Unsigned Integer</td>
		<td>General</td>
	</tr>
	<tr>
		<td>u16</td>
		<td>16-bit Unsigned Integer</td>
		<td>General</td>
	</tr>
	<tr>
		<td>u32</td>
		<td>32-bit Unsigned Integer</td>
		<td>General</td>
	</tr>
	<tr>
		<td>u64</td>
		<td>64-bit Unsigned Integer</td>
		<td>General</td>
	</tr>
	<tr>
		<td>i8</td>
		<td>8-bit Signed Integer</td>
		<td>General</td>
	</tr>
	<tr>
		<td>i16</td>
		<td>16-bit Signed Integer</td>
		<td>General</td>
	</tr>
	<tr>
		<td>i32</td>
		<td>32-bit Signed Integer</td>
		<td>General</td>
	</tr>
	<tr>
		<td>i64</td>
		<td>64-bit Signed Integer</td>
		<td>General</td>
	</tr>
	<tr>
		<td>r32</td>
		<td>32-bit Floating Point Number</td>
		<td>General</td>
	</tr>
	<tr>
		<td>r64</td>
		<td>64-bit Floating Point Number</td>
		<td>General</td>
	</tr>
	<tr>
		<td>b32</td>
		<td>32-bit Boolean</td>
		<td>A bool type with 32-bit sizing.</td>
	</tr>
	<tr>
		<td>b64</td>
		<td>64-bit Boolean</td>
		<td>A bool type with 64-bit sizing.</td>
	</tr>
</table>