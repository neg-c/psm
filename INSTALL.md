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
Additionally, you'll need to install some system-wide packages for development
dependencies.

#### System Dependencies

Before proceeding with the build, install the required system packages for your
distribution:

<details>
<summary><strong>Ubuntu/Debian</strong></summary>

```bash
sudo apt install curl zip unzip tar g++ pkg-config python3-jinja2 libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libglu1-mesa autoconf libltdl-dev libgl-dev
```
</details>

<details>
<summary><strong>Fedora</strong></summary>

```bash
sudo dnf install curl zip unzip tar gcc g++ pkgconfig automake libtool diffutils perl-open.noarch perl python3-jinja2 libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel mesa-libGLU autoconf mesa-libGL-devel
```
</details>

<details>
<summary><strong>Arch Linux</strong></summary>

```bash
sudo pacman -S curl zip unzip tar pkg-config libxrandr libxinerama libxcursor libxi glu autoconf libtool mesa
```
</details>

<details>
<summary><strong>openSUSE</strong></summary>

```bash
sudo zypper install curl zip unzip tar pkg-config libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel Mesa-libGLU autoconf libtool-devel Mesa-libGL-devel
```
</details>

<details>
<summary><strong>CentOS/RHEL</strong></summary>

```bash
sudo yum install curl zip unzip tar pkgconfig libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel mesa-libGLU autoconf libtool-devel mesa-libGL-devel
```
</details>

<details>
<summary><strong>Alpine Linux</strong></summary>

```bash
sudo apk add curl zip unzip tar pkg-config libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev glu autoconf libtool mesa-gl
```
</details>

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

   You can control whether the PSM CLI and GUI tool is built:

   ```bash
   # Disable building PSM CLI (build only the library)
   $ cmake --preset <preset-name> -DBUILD_PSM_CLI=OFF -DBUILD_PSM_GUI=OFF

   # Enable building PSM CLI with GUI (default)
   $ cmake --preset <preset-name> -DBUILD_PSM_CLI=ON -DBUILD_PSM_GUI=ON

   # Disable building PSM GUI (build only the library)
   $ cmake --preset <preset-name> -DBUILD_PSM_GUI=OFF

   # Enable building PSM GUI (default)
   $ cmake --preset <preset-name> -DBUILD_PSM_GUI=ON
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

## Running the Tools

After building, you can run the included tools:

```bash
# Run the command-line tool
$ ./psm_cli --help

# Run the GUI demo tool (if built)
$ ./psm_gui
```

The GUI tool provides an interactive interface for exploring color space conversions and image processing capabilities.
