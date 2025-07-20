# 🖱️ AutoClick DLL Proxy

![Windows](https://img.shields.io/badge/platform-Windows-blue?logo=windows) ![C++](https://img.shields.io/badge/language-C%2B%2B-blue?logo=c%2B%2B) ![DLL](https://img.shields.io/badge/type-DLL-green?logo=microsoft)

A Windows DLL proxy for `dinput8.dll` that automatically clicks at configurable points inside a target window. Useful for automation, games, and testing.

## ✨ Features

- ⚡ Automatic mouse clicks at custom pixel positions
- 🕒 Configure each click for time interval or pixel color
- ➕ Multiple click points supported
- 🪟 Runs as a proxy DLL (`dinput8.dll`) for easy injection
- 🔒 Thread-safe: prevents simultaneous clicks
- 🛠️ Simple configuration via [config.ini](config.ini)

## 🖱️ How It Works

- Injects into the target process as `dinput8.dll`
- Forwards `DirectInput8` calls to the real system DLL
- Reads click configuration from `config.ini`
- ~~Finds the window by its title~~ _(no longer necessary, optional)_
- Spawns a thread for each click point
- Each thread clicks at its position at its own `interval` or when a `color` matches

## ⚙️ Configuration

Configure your click points and behaviors in the [`config.ini`](config.ini) file (no need to recompile):

```ini
# Example configuration for AutoClickProxy

# (Optional) Window title to search for the target window by name
# window_title=SpaceIdle

# Format: x y type parameters
# For type 'time': x y time ms
# For type 'color': x y color r g b

# Example: Time-based click
50 100 time 1000 # Click every second at position (50, 100)

# Example: Color-based click
200 300 color 255 255 255 # Click at position (200, 300) when pixel color is white
400 800 color 0 0 0 # Click at position (400, 800) when pixel color is black
```

**📝 ClickPoint fields**

- `x`, `y`: Pixel position to click (relative to the window's client area)
- `type`: `time` for interval-based, `color` for color-based
- For `time`: `ms` is the interval in milliseconds
- For `color`: `r g b` is the target color (0-255)

## 🛠️ Build

For Windows, it is recommended to use MinGW-w64 (GCC for Windows), but it is not strictly required:

```sh
# Compile as DLL, statically linking C++ libraries
g++ -shared -static-libgcc -static-libstdc++ -static -O2 -o dinput8.dll AutoClickProxy.cpp -lgdi32
```

Alternatively, the [Makefile](Makefile) is available:

```sh
# Run 'make clean' to delete the DLL before building, if needed
make
```

Place the resulting `dinput8.dll` and `config.ini` in the target application's binary folder.

## ⚠️ Warnings

- Use responsibly! Automation may violate terms of service for some applications/games.
- Only use on software you own or have permission to automate.
- This DLL is for educational and testing purposes.

## 📄 License

MIT License. See [LICENSE](LICENSE) for details.
