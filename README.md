# OpenGL Tutorials

A progressive collection of OpenGL 3.3 Core Profile examples, each demonstrating specific graphics programming concepts — from opening a window to shadow mapping.

Heavily influenced by the tutorials at [learnopengl.com](https://learnopengl.com/). Built with C++, CMake, and a consistent set of vendored dependencies.

## Projects

| # | Project | Concepts |
|---|---------|----------|
| 1 | [glfw](glfw/) | Window creation, OpenGL context setup, render loop |
| 2 | [triangle-basic](triangle-basic/) | VAO/VBO, vertex attributes, shader compilation, `glDrawArrays` |
| 3 | [triangle](triangle/) | Element Buffer Objects (EBO), indexed rendering with `glDrawElements` |
| 4 | [rectangle](rectangle/) | Interleaved vertex attributes (position, color, UVs), stride/offset layout |
| 5 | [shaders](shaders/) | Uniform variables, passing per-frame data to shaders |
| 6 | [transformations-basic](transformations-basic/) | Model/View/Projection matrices with GLM |
| 7 | [transformations](transformations/) | 3D cube rendering, depth testing, multi-texturing, mipmapping |
| 8 | [textures](textures/) | Texture wrapping modes, filtering, mipmaps, texture units |
| 9 | [cubemaps](cubemaps/) | Skybox rendering, cubemap sampling, camera with mouse look |
| 10 | [light/materials](light/materials/) | Phong lighting model, material properties (ambient/diffuse/specular) |
| 11 | [light/lighting-maps](light/lighting-maps/) | Diffuse and specular maps for per-pixel material variation |
| 12 | [light/all-light-types](light/all-light-types/) | Directional, point, and spot lights with attenuation |
| 13 | [framebuffers](framebuffers/) | Off-screen rendering (FBO), render-to-texture, post-processing |
| 14 | [shadows/directional](shadows/directional/) | Shadow mapping, depth maps, PCF soft shadows |
| 15 | [shadows/point](shadows/point/) | Omnidirectional shadow mapping with cubemap depth textures |

## Dependencies

All dependencies are vendored per-project — no system-wide installation needed.

- **[GLFW 3.4](https://www.glfw.org/)** — Window and input
- **[GLAD](https://glad.dav1d.de/)** — OpenGL function loader
- **[GLM](https://github.com/g-truc/glm)** — Math library (vectors, matrices, transformations)
- **[stb_image](https://github.com/nothings/stb)** — Image loading (projects that use textures)

## Building

Each project builds independently with CMake.

```bash
cd <project-directory>
cmake --build build
./build/main
```

Requires CMake 3.10+ and a C++17 compiler.

## Platform Notes

- Tested on macOS — all projects include `GLFW_OPENGL_FORWARD_COMPAT` for Apple compatibility
- Uses OpenGL 3.3 Core Profile
## Structure

Each project is self-contained with the same layout:

```
project/
├── main.cpp              # Entry point
├── CMakeLists.txt        # Build config
├── assets/
│   ├── shaders/          # GLSL vertex/fragment shaders
│   └── textures/         # Images (where applicable)
└── vendor/               # Vendored dependencies
    ├── glfw-3.4/
    ├── glad/
    └── glm/
```

Some later projects also include reusable helper classes:

- `shader.h` — Shader program wrapper with uniform setters
- `camera.h` — FPS-style camera with keyboard/mouse input
