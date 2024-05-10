# ticalc

<div style="display: flex;">
    <img src="https://github.com/cococry/ticalc/blob/main/branding/ticalc-showcase.png" style="flex: 50%; padding: 20px;" width="250px">
</div>

## Overview

Ticalc is built to be very small and minimalistic while still retaining
aesthetics and functionality. The codebase is contained in only about 450 lines of code that are
structured in 2 .c files.

The UI enigne/library used for the interface is [libleif](https://github.com/cococry/leif) which is
a minimalistic stb-like immediate mode UI library. (~3500 lines of code). The library uses OpenGL
as a graphics backend and [glfw](https://github.com/glfw/glfw) is used for window handling.

The calculator is highly configurable and easily extendable due to its very small and readable codebase.
As a sidenote, the entire application can be used by only using keyboard shortcuts and number input.

## Installation

- Install the dependencies


On arch:
```console
paru -S make gcc cglm libclipboard glfw
```

On debian:


Note: You will have to build libclipboard yourself.
Follow the instructions [here](https://github.com/jtanx/libclipboard?tab=readme-ov-file#building).
```console
sudo apt-get install make gcc libglfw3 libglfw3-dev libcglm-dev
```

- Clone the repository
```console
git clone https://github.com/cococry/ticalc
cd ticalc
```

- Build & Install

```console
sudo make && sudo make install
```

## Uninstalling

```console
sudo make uninstall
```

## Running

There is a .desktop file that is automatically installed on install that you can
access or just run the binary via terminal:

```console
ticalc
```

## Build Dependencies

| Dependency         |  Reason of Usage    |
| ----------------|-------------|
| [leif](https://github.com/cococry/leif) | Creating the entire UI Frontend |
| [libm](https://en.wikipedia.org/wiki/C_mathematical_functions#libm) | Mathematical functions |
| [llibclipboard](https://github.com/jtanx/libclipboard) | Handling clipboard |
| [libGL](https://dri.freedesktop.org/wiki/libGL/) | OpenGL Functionality |
| [GLFW](https://github.com/glfw/glfw) | Handling windowing, input etc. |
| [glad](https://github.com/Dav1dde/glad) | Loading OpenGL functions |
| [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) | Loading image files into memory |
| [stb_image_resize2](https://github.com/nothings/stb/blob/master/stb_image_resize2.h) | Resizing images |
| [stb_truetype](https://github.com/nothings/stb/blob/master/stb_truetype.h) | Loading font glyphs from font files |
| [cglm](https://github.com/recp/cglm) | Linear Algebra Math |

## Lines of Code

| File         |  Lines of Code |
| -------------|----------------|
| ticalc.c     | 345            |
| config.h     | 43             |
| eval.h       | 2              |
| eval.c       | 73             |
| **Total**    | 463            |

## Contributing
You can contribute to ticalc by:
  - [Fixing bugs or contributing to features](https://github.com/cococry/ticalc/issues)
  - [Changing features or adding new functionality](https://github.com/cococry/ticalc/pulls)
