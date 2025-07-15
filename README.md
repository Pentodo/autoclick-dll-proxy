# 🖱️ AutoClick DLL Proxy

![Windows](https://img.shields.io/badge/platform-Windows-blue?logo=windows) ![C++](https://img.shields.io/badge/language-C%2B%2B-blue?logo=c%2B%2B) ![DLL](https://img.shields.io/badge/type-DLL-green?logo=microsoft)

A Windows DLL proxy for `dinput8.dll` that automatically clicks at configurable points inside a target window. Useful for automation, games, and testing.

## ✨ Features

- ⚡ **Automatic Mouse Clicks** at custom X/Y positions
- 🕒 **Independent Intervals** for each click point
- 🛡️ **No Overlapping Clicks** (uses coprime intervals)
- 🪟 **Client Area Coordinates** (ignores window title bar)
- 🔗 **DirectInput8 Proxy** (forwards calls to original system DLL)

## 🖱️ How It Works

- Injects into the target process as `dinput8.dll`
- Finds the window by its title (`SpaceIdle` by default)
- Spawns a thread for each click point
- Each thread clicks at its position at its own interval
- Forwards DirectInput8 calls to the real system DLL

## ⚙️ Configuration

Edit the `ClickPoint` array in `AutoClickProxy.cpp`:

```cpp
// The wait_ms values below are chosen so that clicks never overlap.
// They are not multiples of each other and do not share common divisors.
const ClickPoint clickPoints[] = {
    {30, 834, 45001},
    {100, 834, 5003},
    {170, 834, 45011}
};
```

- `x`, `y`: Coordinates relative to the window's client area (not including the title bar)
- `wait_ms`: Interval in milliseconds for each click point

## 🛠️ Build

For Windows, it is recommended to use MinGW-w64 (GCC for Windows), but it is not strictly required:

```sh
# Compile as DLL, statically linking C++ libraries
g++ -shared -static-libgcc -static-libstdc++ -static -O2 -o dinput8.dll AutoClickProxy.cpp
```

Alternatively, the Makefile is available:

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
