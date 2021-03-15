# RTek Game Engine [![Build status](https://ci.appveyor.com/api/projects/status/vhg77c4q2dan0l16?svg=true)](https://ci.appveyor.com/project/sdsmith/rtek)

Experimental game engine.

## Building the project

### Platform: Windows

> Note: The commands that follow assume you are working in Cygwin.

Visual Studio solution:
```sh
cmake -S. -B./build -G"Visual Studio 16 2019"
```

The solution is present in the `build` folder.

To make a compilation database (`compile_commands.json`) for external tools:
```sh
cmake -S. -B./build -G"NMake Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

Only the configure step is required to make the compilation database. For ease of integration, make a symlink to it in the root of the project:
```sh
ln -s ./build/compile_commands.json compile_commands.json
```

### Dependency Installation

> Note: Dependency installation is not necessary. However, it will result in dependencies being downloaded each time the build directory is blown away.

> Warning: Changes to dependency configurations will require a deletion of the installed dependency and a rebuild.

To avoid having dependencies re-downloaded when the `build` directory is deleted, dependencies can be installed through cmake.

```sh
# Configure cmake with generator of choice, for example:
cmake -S. -B./build -G"Visual Studio 16 2019"

# Build
cmake --build build

# Install dependencies
cmake --install build
```

By default this installs them to a `_deps` in the root of the repository.

Next time cmake configuration is done, the packages will be picked up from the `_deps` location. Very helpful if you need to blow away `build` or when a file is added/removed from the project.

## Feature Toggles

These are done through preprocessor defines and/or cmake options.

See `CMakeLists.txt` options for the available feature toggles.

General:
- `RK_ASSERTIONS_ENABLED`: If present, enable assertions. Otherwise they are compiled out. Note, critical assertions are always enabled.

Logging:
- `RK_LOG_LEVEL`: Log level compiled into the application. One of `RK_LOG_LEVEL_{OFF, TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL}`. Levels listed in order of least to most precendence. Levels of equals or less precedence are removed.
- `RK_LOGGING_OFF`: Remove all logging. Takes precedence over `RK_LOG_LEVEL`.
- `RK_LOGGING_PERF`: Compile in minimal logging (for performance). Takes precedence over `RK_LOG_LEVEL`.

Graphics:
- `RK_OGL_DEBUG`: Display debug messages from OpenGL, including errors.
- `RK_SHADER_BASE_DIR`: Directory containing all the shaders. Prepended to the path of the shader being opened.
- `RK_REQUEST_HIGH_PERF_RENDERER`: Ask for a high performance renderer. For systems with an iGPU and dGPU, this typically means the dGPU.

## Coding Conventions
Engine macros are prefixed with `RK_`. Engine internal macros that should not be used are prefixed with `RK_I_`.

## Unicode

The engine is unicode only and does not support ANSI strings. It provides its own cross platform abstraction for unicode strings, `uchar` (read "unicode character") and the string litteral prefix `UC` (read "unicode"). The unicode chracter format use on each platform corresponds to:

| Platform | Unicode Encoding |
|-|-|
| Windows | UTF-16 |
| Linux | UTF-8 |

This means that on Linux `uchar` is `char`. Be careful when developing Windows compatible unicode functions when working in Linux.

Using UTF-16 on Windows means that the native Windows unicode functions can be used directly. For Windows, `uchar` is essentially `WCHAR`. And since this engine is always using UNICODE, it is also equivalent to `TCHAR`.
