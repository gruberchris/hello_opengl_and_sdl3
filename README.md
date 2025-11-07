# OpenGL 3D Cube with SDL3

A simple 3D graphics application that renders a colorful rotating cube using OpenGL and SDL3.

## Description

This application demonstrates basic 3D rendering with OpenGL and SDL3. It displays a cube with smooth color gradients on each face, positioned in front of a camera. The cube can be rotated and the camera can be zoomed in and out using keyboard controls.

Each face of the cube features a unique color gradient:
- **Front**: Red to Yellow
- **Back**: Blue to Cyan
- **Top**: Green to Yellow
- **Bottom**: Magenta to Purple
- **Right**: Orange to Pink
- **Left**: Teal to Lime

## Libraries Used

### SDL3 (Simple DirectMedia Layer)
Handles the application infrastructure:
- Window creation and management
- OpenGL context initialization
- Input handling (keyboard events)
- Event loop and polling
- Buffer swapping for display
- Frame timing for smooth animation

### OpenGL
Handles the 3D graphics rendering:
- Drawing cube geometry with vertices
- Color gradient interpolation across faces
- Depth testing for correct face ordering
- Projection matrix for 3D perspective
- Transformations (rotation, camera positioning)
- Graphics pipeline processing

**Platform-specific OpenGL versions:**
- macOS: OpenGL 4.1 (highest version supported)
- Linux/Windows: OpenGL 4.6

## Controls

| Key | Action |
|-----|--------|
| **R** | Start cube rotation |
| **T** | Stop cube rotation |
| **+** or **=** | Zoom camera in |
| **-** | Zoom camera out |
| **ESC** | Exit application |

## Building

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- SDL3 development libraries
- OpenGL development libraries

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .

# Run the application
./hello_opengl_and_sdl3
```

## Output

On startup, the application prints version information to the console:
- SDL3 version number
- OpenGL version and renderer information
- Control instructions

## Platform Notes

- **macOS**: Uses OpenGL 4.1 Core Profile (forward compatible)
- **Linux**: Uses OpenGL 4.6 Core Profile
- **Windows**: Uses OpenGL 4.6 Core Profile

The application window is resizable and maintains proper aspect ratio during resizing.
