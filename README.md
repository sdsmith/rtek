# RTek Game Engine [![Build status](https://ci.appveyor.com/api/projects/status/vhg77c4q2dan0l16?svg=true)](https://ci.appveyor.com/project/sdsmith/rtek)

Experimental game engine.

## Building the project

### Windows

> Note: The commands that follow assume you are working in Cygwin.

Visual Studio solution:
```
cmake -S. -B./build -G"Visual Studio 16 2019"
```

The solution is present in the `build` folder.

To make a compilation database (`compile_commands.json`) for external tools:
```
cmake -S. -B./build -G"NMake Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

Only the configure step is required to make the compilation database. For ease of integration, make a symlink to it in the root of the project:
```
ln -s ./build/compile_commands.json compile_commands.json
```

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
