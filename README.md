# Shaderscope

Shaderscope is a playground for cultivating and observing artificial life and other interesting algorithms. Think of it like a digital petri dish in which you can throw your ideas and then poke around to see what happens.

Current libraries in use:
- [SDL2](https://www.libsdl.org/index.php) for the windowing system
- [bgfx](https://github.com/bkaradzic/bgfx) for the graphics library
- [Dear ImGui](https://github.com/ocornut/imgui) for the user interface.
- BIMS for utility and as a framework around the above libraries.

Build configuration and is based on [sdl-bgfx-imgui-starter](https://github.com/pr0g/sdl-bgfx-imgui-starter).

## Setup

Clone the repo:

```bash
git clone https://github.com/Shuuby/shaderscope.git
```

This project comes with a _superbuild_ CMake script which will **download** and build all third party dependencies and the main application in one step. The third party dependencies are built and installed to a separate build folder in the third party directory. To achieve this CMake must be installed on your system (the repo has most recently been tested with CMake version `3.24`).

The libraries are by default self contained in the repo and are not installed to the system.

## Windows build steps

The project is being developed solely on and for Windows for the time being. Support for other platforms will be added eventually.

> **Note**
> A number of `configure-<generator>.bat/sh` files are provided to run the CMake configure commands. `Ninja` is preferred as it's consistent across _macOS_, _Linux_ and _Windows_ (and it's very fast), any generator should work though. There are a few utility batch files included to ease development.

1. Generate ninja build configuration files with
```bash
configure-ninja.bat
```
2. Build targets with
   
> **Warning**
> Ninja might throw permission errors on this step. You can simply run the command again to proceed.
> 
```bash
cmake --build build\debug-ninja 
```
and/or
```bash
cmake --build build\release-ninja
```
3. Compile shaders with
```bash
compile-shaders-win.bat
```

Launch the built application with `build\debug-ninja\src\shaderscope\Shaderscope.exe` or `build\release-ninja\src\shaderscope\Shaderscope.exe` 

## Thank you
- [pr0g](https://github.com/pr0g) for [sdl-bgfx-imgui-starter](https://github.com/pr0g/sdl-bgfx-imgui-starter)
- [Бранимир Караџић (@bkaradzic)](https://twitter.com/bkaradzic) for [bgfx](https://github.com/bkaradzic/bgfx)
- [Omar Cornut (@ocornut)](https://twitter.com/ocornut) for [Dear ImGui](https://github.com/ocornut/imgui)
- [Widberg/MissingBitStudios](https://github.com/widberg) for the `bgfx` CMake support
- [Tamas Kenez](https://github.com/tamaskenez) for the `Dear ImGui` CMake support
- [Richard Gale (@richardg4)](https://twitter.com/richardg4) for the `bgfx` implementation for `Dear ImGui`
  
  