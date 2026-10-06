# OpenGL Voxel Engine

A voxel terrain engine using OpenGL 3.3, GLFW, GLM, Dear ImGui and libnoise.

## Layout
<img width="789" height="593" alt="image" src="https://github.com/user-attachments/assets/299986fd-cf26-4f94-b8e1-058356903563" />


```
src/            engine source code
assets/         shaders and textures (copied next to the executable on build)
third_party/    bundled libraries: glad, glm, imgui, stb_image
CMakeLists.txt  build script
```

GLFW and libnoise are downloaded automatically by CMake the first time you configure, so you need an internet connection for the first build.

## Requirements

- CMake 3.20 or newer
- A C++17 compiler (MSVC 2019+, GCC 9+ or Clang 10+)
- Git (CMake uses it to download dependencies)
- **Linux only:** the libraries GLFW needs to build:
  - Debian/Ubuntu: `sudo apt install libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libwayland-dev libxkbcommon-dev wayland-protocols`
  - Fedora: `sudo dnf install mesa-libGL-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel wayland-devel libxkbcommon-devel`

## Building

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The executable and a copy of `assets/` end up in `build/` (or `build/Release/` with Visual Studio). Run it from that folder so it can find `shaders/` and `textures/`:

```sh
cd build        # or build/Release
./VoxelEngine
```

### Visual Studio

Either open the folder directly (**File > Open > Folder**, and Visual Studio picks up `CMakeLists.txt`), or generate a solution:

```sh
cmake -S . -B build -G "Visual Studio 17 2022"
```

Then open `build/VoxelEngine.sln`. `VoxelEngine` is already set as the startup project, with its working directory set to the executable's folder.

### Using an installed GLFW

To use a GLFW you already have installed instead of downloading it, pass `-DVOXEL_USE_SYSTEM_GLFW=ON`. It must be built for the same compiler you are using.

## Controls

| Key | Action |
| --- | --- |
| W A S D | Move |
| Mouse | Look |
| Scroll | Zoom |
| E | Toggle the settings UI (frees the cursor) |
| / | Toggle wireframe |
| Esc | Quit |
