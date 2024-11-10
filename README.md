# psm

Prisma - color space conversion library

## Table of Contents

- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Build and Installation](#build-and-installation)
- [Usage](#usage)
- [License](#license)

## Getting Started

### Prerequisites

- CMake (v3.25 or higher)
- Ninja
- vcpkg

For **Debian** like systems:

```bash
$ sudo apt install cmake ninja-build
```

### Build and Installation

```bash
# List the provided presets
$ cmake --list-presets
# Then configure with
$ cmake --preset <chosen-preset>
# And Build
$ cmake --build build/<chosen-preset>
```

# Usage

```bash
# There is an application that uses psm
$ ./build/<chosen-preset>/src/app/psm_cli
```

# License

[MIT License](LICENSE)
