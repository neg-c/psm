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

On Linux, `vcpkg` is not pre-installed, so you'll need to install it manually.
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

   By default, this will build all available modules. To build only specific
   modules, use the WITH flags:

   ```bash
   # Build with only oRGB support
   $ cmake --preset <preset-name> -DWITH_ORGB=ON

   # Build with only Adobe RGB support
   $ cmake --preset <preset-name> -DWITH_ADOBE_RGB=ON

   # Build with both modules
   $ cmake --preset <preset-name> -DWITH_ORGB=ON -DWITH_ADOBE_RGB=ON
   ```

   You can control whether the PSM CLI tool is built:

   ```bash
   # Disable building PSM CLI (build only the library)
   $ cmake --preset <preset-name> -DBUILD_PSM_CLI=OFF

   # Enable building PSM CLI (default)
   $ cmake --preset <preset-name> -DBUILD_PSM_CLI=ON
   ```

   You can also control whether tests are built:

   ```bash
   # Disable building tests
   $ cmake --preset <preset-name> -DBUILD_TESTING=OFF

   # Enable building tests (default)
   $ cmake --preset <preset-name> -DBUILD_TESTING=ON
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

## Testing

When tests are enabled (which is the default), you can run them after building:

```bash
$ cd build/<preset-name>
$ ctest
```

Note that tests are only built for modules that are being built. For example, if
you only build the Adobe RGB module with `-DWITH_ADOBE_RGB=ON`, only the Adobe
RGB tests will be available.
