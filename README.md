# 🌌 3D Solar System Simulation

A fully interactive 3D solar system built with **C++ and Modern OpenGL (3.3 Core Profile)**.

## 📸 Features

- **Realistic Planets**: 8 textured planets orbiting the Sun with proper axial tilt
- **Phong Lighting**: Ambient + Diffuse + Specular illumination from the Sun
- **Skybox**: Star-field background (loaded texture or procedural fallback)
- **Free Camera**: FPS-style navigation with mouse look and keyboard movement
- **Graphics Algorithms**: Midpoint Circle (orbits), DDA (axes), Bresenham (grid)
- **2D Transformations**: Translation, Rotation, Scaling, Reflection, Shear demos
- **Interactive Controls**: Pause, speed control, planet selection, orbit toggles

## 🔧 Prerequisites

- **CMake** 3.16+
- **C++17** compiler (MSVC, GCC, or Clang)
- **Python 3** (for GLAD generation during setup)
- **Git** (for CMake FetchContent)

## 🚀 Setup & Build

### Step 1: Run Setup Script
```powershell
cd c:\DIU\CG\solarSystem
powershell -ExecutionPolicy Bypass -File setup.ps1
```
This downloads GLAD, stb_image, and planet textures.

### Step 2: Build with CMake
```powershell
cd build
cmake ..
cmake --build . --config Release
```

### Step 3: Run
```powershell
.\Release\SolarSystem.exe
```

## 🎮 Controls

| Key | Action |
|-----|--------|
| `W/A/S/D` | Move camera forward/left/backward/right |
| `Q/E` | Move camera up/down |
| `Mouse` | Look around |
| `Scroll` | Zoom in/out |
| `Shift` | Sprint (3x speed) |
| `O` | Toggle orbit paths |
| `P` | Pause/Resume simulation |
| `+/-` | Increase/Decrease simulation speed |
| `[/]` | Select previous/next planet |
| `F` | Focus camera on selected planet |
| `I` | Print planet info to console |
| `X` | Toggle axis lines (DDA algorithm) |
| `G` | Toggle grid (Bresenham algorithm) |
| `T` | Toggle transformation demo (Reflection + Shear) |
| `ESC` | Exit |

## 📂 Project Structure

```
solarSystem/
├── CMakeLists.txt          # Build system
├── setup.ps1               # Dependency & texture downloader
├── README.md
├── include/                # Header files
│   ├── shader.h
│   ├── camera.h
│   ├── sphere.h
│   ├── planet.h
│   ├── skybox.h
│   ├── orbit_renderer.h
│   ├── solar_system.h
│   └── algorithms/
│       ├── dda.h
│       ├── bresenham.h
│       └── midpoint_circle.h
├── src/                    # Source files
│   ├── main.cpp
│   ├── shader.cpp
│   ├── camera.cpp
│   ├── sphere.cpp
│   ├── planet.cpp
│   ├── skybox.cpp
│   ├── orbit_renderer.cpp
│   ├── solar_system.cpp
│   └── algorithms/
│       ├── dda.cpp
│       ├── bresenham.cpp
│       └── midpoint_circle.cpp
├── shaders/                # GLSL shaders
│   ├── planet.vert/frag    # Phong lighting + textures
│   ├── sun.vert/frag       # Emissive glow
│   ├── skybox.vert/frag    # Star background
│   └── line.vert/frag      # Orbits, axes, grid
├── textures/               # Planet textures (downloaded)
└── vendor/                 # Third-party (GLAD, stb_image)
```

## 📌 Academic Requirements Fulfilled

| Requirement | Where |
|------------|-------|
| Points, Lines, Polygons | Orbit paths (LINE_LOOP), axes (POINTS), spheres (TRIANGLES) |
| Midpoint Circle Algorithm | `src/algorithms/midpoint_circle.cpp` → orbit generation |
| DDA Line Algorithm | `src/algorithms/dda.cpp` → axis visualization |
| Bresenham Line Algorithm | `src/algorithms/bresenham.cpp` → grid generation |
| Translation | Planet orbit position (`planet.cpp:getModelMatrix`) |
| Rotation | Axial spin + orbital revolution (`planet.cpp`) |
| Scaling | Planet radii (`planet.cpp:getModelMatrix`) |
| Reflection | Mirror demo across XZ plane (`planet.cpp:getReflectedModelMatrix`) |
| Shear | Shear demo (`planet.cpp:getShearedModelMatrix`) |
| Animation | Delta-time based continuous loop (`main.cpp`) |
| Phong Lighting | `shaders/planet.frag` (Ambient + Diffuse + Specular) |
| Texture Mapping | UV coordinates on sphere mesh (`sphere.cpp`) |
| Camera System | FPS camera with mouse/keyboard (`camera.cpp`) |
| Skybox | Star sphere with texture (`skybox.cpp`) |
| Modern OpenGL | VAO/VBO/EBO, Core Profile 3.3 |

## 📜 License

Educational project. Planet textures from [Solar System Scope](https://www.solarsystemscope.com/textures/) (CC BY 4.0).
