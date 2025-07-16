# 🖱️ AutoClick DLL Proxy

![Windows](https://img.shields.io/badge/platform-Windows-blue?logo=windows) ![C++](https://img.shields.io/badge/language-C%2B%2B-blue?logo=c%2B%2B) ![DLL](https://img.shields.io/badge/type-DLL-green?logo=microsoft)

A Windows DLL proxy for `dinput8.dll` that automatically clicks at configurable points inside a target window. Useful for automation, games, and testing.

## ✨ Features

- ⚡ **Automatic Mouse Clicks** at custom X/Y positions
- 🕒 **Independent Intervals** for each click point
- 🛡️ **No Overlapping Clicks** (uses coprime intervals)
- 🪟 **Client Area Coordinates** (ignores window title bar)
- 🔗 **DirectInput8 Proxy** (forwards calls to original system DLL)
- 🎨 **Color-Based Clicks**: Click only when a pixel matches a specific color
- 🧩 **Flexible Click Behavior**: Choose between time-based or color-based clicking for each point

## 🖱️ How It Works

- Injects into the target process as `dinput8.dll`
- Finds the window by its title (`SpaceIdle` by default)
- Spawns a thread for each click point
- Each thread clicks at its position at its own interval or when a color matches
- Forwards DirectInput8 calls to the real system DLL

## ⚙️ Configuration

Edit the `ClickPoint` array and behaviors in `AutoClickProxy.cpp` as desired:

### ⏱️ Time-Based Clicking

Use independent intervals for each point:

```cpp
// Example 1: Time-based click behaviors
TimeClickBehavior shieldBoostInterval(45001);
TimeClickBehavior laserBoostInterval(5003);
TimeClickBehavior kineticVolleyInterval(45011);

ClickPoint clickPoints[] = {
    {30, 834, &shieldBoostInterval},
    {100, 834, &laserBoostInterval},
    {170, 834, &kineticVolleyInterval}
};
```

### 🎨 Color-Based Clicking

Click only when the pixel color matches:

```cpp
// Example 2: Color-based click behavior
ColorClickBehavior white(RGB(255, 255, 255));

ClickPoint clickPoints[] = {
    {31, 807, &white},
    {99, 824, &white},
    {167, 817, &white}
};
```

**Parameters:**

- `x`, `y`: Horizontal and vertical position of the click, relative to the window's client area (the content area, not including the title bar or window borders)
- `behavior`: Pointer to a click behavior (time-based or color-based)

## 🛠️ Build

For Windows, it is recommended to use MinGW-w64 (GCC for Windows), but it is not strictly required:

```sh
# Compile as DLL, statically linking C++ libraries
g++ -shared -static-libgcc -static-libstdc++ -static -O2 -o dinput8.dll AutoClickProxy.cpp
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
