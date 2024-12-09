# INSTALL

## Prerequisites

To build and use Prisma (`psm`), ensure your system meets the following
requirements:

- **Operating System**: Linux or Windows
- **Compiler**: GCC 13+, Clang 16+, or MSVC 19.29+
- **CMake**: Version 3.27 or higher
- **vcpkg**: Required for dependency management

### Windows

On Windows, Visual Studio comes with `vcpkg` integrated. You can proceed to the
**Building** section directly without needing to install `vcpkg` separately.

### Linux

On Linux, `vcpkg` is not pre-installed, so you’ll need to install it manually.
Follow these steps to set it up, then proceed to the **Building** section.

### Install vcpkg

The recommended way to install vcpkg is in your home directory:

```bash
$ git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
$ ~/vcpkg/bootstrap-vcpkg.sh
$ ~/vcpkg/vcpkg integrate install
$ echo "export VCPKG_ROOT=~/vcpkg" >> ~/.bashrc
$ source ~/.bashrc
```

## Building

Prisma includes pre-defined CMake presets to simplify the configuration process.
Assuming that the VCPKG_ROOT is set:

1. List Available presets
   ```bash
   $ cmake --list-presets
   ```
2. Use a preset to configure
   ```bash
   $ cmake --preset <preset-name>
   ```
3. Build the project
   ```bash
   $ cmake --build build/<preset-name>
   ```

## Installing

To install Prisma system-wide, run:

```bash
cmake --install <build-directory>/<preset-name>
```
