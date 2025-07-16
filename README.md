# 🖱️ AutoClick DLL Proxy

![Windows](https://img.shields.io/badge/platform-Windows-blue?logo=windows) ![C++](https://img.shields.io/badge/language-C%2B%2B-blue?logo=c%2B%2B) ![DLL](https://img.shields.io/badge/type-DLL-green?logo=microsoft)

A Windows DLL proxy for `dinput8.dll` that automatically clicks at configurable points inside a target window. Useful for automation, games, and testing.

## ✨ Features

- ⚡ Automatic mouse clicks at custom pixel positions
- 🕒 Configure each click for time interval or pixel color
- ➕ Multiple click points supported
- 🪟 Runs as a proxy DLL (`dinput8.dll`) for easy injection
- 🔒 Thread-safe: prevents simultaneous clicks
- 🛠️ Simple configuration in [AutoClickProxy.cpp](AutoClickProxy.cpp)

## 🖱️ How It Works

- Injects into the target process as `dinput8.dll`
- Finds the window by its title (`SpaceIdle` by default)
- Spawns a thread for each click point
- Each thread clicks at its position at its own interval or when a color matches
- Forwards DirectInput8 calls to the real system DLL

## ⚙️ Configuration

Define your click points and behaviors in [AutoClickProxy.cpp](AutoClickProxy.cpp):

```cpp
// Time-based click
TimeClickBehavior fiveSeconds(5000);

// Color-based click
ColorClickBehavior white(RGB(255, 255, 255));

// Example using both methods
ClickPoint clickPoints[] = {
    {30, 640, &fiveSeconds},    // time-based
    {100, 640, &fiveSeconds},   // time-based
    {170, 800, &white}          // color-based
};
```

**📝 ClickPoint attributes**

- `x`, `y`: Horizontal and vertical position of the pixel to click (relative to the window's client area; does not include the title bar or window borders)
- `behavior`: Pointer to a click behavior (`TimeClickBehavior` or `ColorClickBehavior`)

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

Place the resulting `dinput8.dll` in the target application's binary folder.

## ⚠️ Warnings

- Use responsibly! Automation may violate terms of service for some applications/games.
- Only use on software you own or have permission to automate.
- This DLL is for educational and testing purposes.

## 📄 License

MIT License. See [LICENSE](LICENSE) for details.
