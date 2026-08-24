# Drux Lord

A Linux clone of the classic game [Drug Lord 2](http://www.geekhideout.com/druglord2.shtml) built with modern C++20 and Qt 6 (or Qt 5).

## Description

Drux Lord is a turn-based strategy simulation game where players buy and sell commodities across various international cities, manage loans and finances at the bank, upgrade weapons and equipment, and recover at local hospitals.

## Prerequisites

To build Drux Lord, ensure you have the following installed on your system:

- **C++ Compiler** supporting C++20 (GCC 11+, Clang 13+, or MSVC)
- **CMake** (version 3.24 or higher)
- **Qt 6 or Qt 5** development libraries (`qt6-base-dev` or `qtbase5-dev` on Debian/Ubuntu)

On Debian/Ubuntu-based distributions:
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev
```

## Building and Running

### 1. Configure
```bash
cmake -B build -S .
```

### 2. Compile
```bash
cmake --build build
```

The compiled binary will be located in `build/bin/druxlord`.

### 3. Run
```bash
./build/bin/druxlord
```

### 4. Install (Optional)
```bash
cmake --install build --prefix /usr/local
```

## License

GPL v2 - see [copying](copying) for details.

## Authors & Credits

- **Original Author**: Ardhan Madras <ajhwb@knac.com>
