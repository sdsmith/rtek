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

### Developing the dependencies in parallel

To develop one or more of the dependencies while working on RTek, point FetchContent to the local repo by overriding the cmake cache value.

For example, to develop sdslib:
```sh
cmake -S. -B./build -G"Visual Studio 16 2019" -DFETCHCONTENT_SOURCE_DIR_SDSLIB=<PATH_TO_REPO_ROOT>
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

## Coding Conventions
Engine macros are prefixed with `RK_`. Engine internal macros that should not be used are prefixed with `RK_I_`.

## Unicode

All internal representation of strings and text output are UTF-8. The engine is unicode only and does not support ANSI strings. It follows the principles of [UTF-8 everywhere](https://utf8everywhere.org).

> NOTE: UTF-8 is backwards compatible with ASCII[^not-extended-ascii]. Therefore searching for ASCII characters/strings in UTF-8 strings is a valid.

[^not-extended-ascii]: ASCII is not Extended ASCII! ASCII encodes characters from 0-127.

**Programming guidelines:**
- `std::string` and `char*` variables are considered UTF-8, anywhere in the program.
- Use what's available in `platform/stdlib/*` over those in the standard library. These functions take UTF-8 strings.[^winstdlib-utf8]
- Do not use `wchar_t` or `std::wstring` in any place other than adjacent point to APIs accepting UTF-16.
- Do not use `L""` literals in any place other than parameters to APIs accepting UTF-16.
- Windows:
  - Do not use types, functions, or their derivatives that are sensitive to the `UNICODE` constant, such as `LPTSTR`, `CreateWindow()` or the `_T()` macro. Instead, use `LPWSTR`, `CreateWindowW()`, and explicit `L""` literals.
  - `UNICODE` and `_UNICODE` are always defined to avoid passing narrow UTF-8 strings to ANSI WinAPI getting silently compiled.
  - Only use Win32 functions that accept widechars (`LPWSTR`), never those which accept `LPTSTR` or `LPSTR`. Pass parameters this way:
     ```cpp
     std::array<wchar_t, MAX_PATH> wpath;
     if (!unicode::widen(wpath.data(), wpath.size(), some_string)) {
         return Status::unicode_error;
     }
     SetWindowTestW(wpath.data());
     ```
     - Output from windows API functions require `unicode::narrow` to re-encode to UTF-8 for use internally.

**Working with files, filenames, and fstreams:**
- Always produce text output files in UTF-8.
- Use what's available in `platform/stdlib/*` over those in the standard library. These functions take UTF-8 strings.[^winstdlib-utf8]

[^winstdlib-utf8]: Windows does not make their standard library implementation UTF-8 aware (`iostream`, `fstream`, ...). Using the ones provided allows for proper UTF-8 handling across platforms.
