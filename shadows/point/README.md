## The Complete Flow

```
CPU sends one triangle (local space vertices)
           ↓
VERTEX SHADER (runs 3 times, once per vertex)
  → multiplies by model matrix
  → outputs world space position
           ↓
GEOMETRY SHADER (runs once per triangle)
  → receives 3 world space vertices
  → loops 6 times (once per cubemap face)
      → sets gl_Layer to direct output to correct face
      → transforms vertices by that face's light space matrix
      → emits triangle to that face
  → outputs 6 triangles total (18 vertices)
           ↓
RASTERIZATION (GPU automatically interpolates FragPos across triangles)
           ↓
FRAGMENT SHADER (runs once per pixel covered)
  → receives interpolated world space position
  → calculates real distance from light
  → normalizes to [0,1]
  → writes to depth buffer of that cubemap face
           ↓
RESULT: depth cubemap with all 6 faces filled
        each face stores normalized linear distance
        from the light to the nearest surface
```