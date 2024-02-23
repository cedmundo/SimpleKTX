# SimpleKTX

A usage example of [KTX-Software library](https://github.com/KhronosGroup/KTX-Software), which
only loads a KTX1 file into GPU.

## Building

First install all dependencies needed to build GLFW3 and libKTX from source, then fetch code with submodules:

```sh
git clone --recurse-submodules https://github.com/cedmundo/SimpleKTX.git
```

Build with CMake:
```sh
mkdir build
cmake -B build -S .
```

Done.
