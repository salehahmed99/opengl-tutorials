# Shadow Mapping — Complete Chapter Summary

---

## 1. Core Concept

Shadows are the **absence of light due to occlusion**. The fundamental idea behind shadow mapping is:

> **If you stood at the light source and looked at the scene, anything you can see is lit. Anything hidden from your view is in shadow.**

Shadow mapping is a **two-pass technique** that reuses the GPU's depth buffer machinery to approximate this.

---

## 2. The Two-Pass Pipeline

### Pass 1 — Generate the Depth Map
Render the scene from the **light's point of view** using a simple depth-only shader. Store the resulting depth values in a texture called the **shadow map** (or depth map). This records the closest depth the light can see in every direction.

### Pass 2 — Render the Scene with Shadows
Render the scene normally from the **camera's point of view**. For every fragment, ask: *"is this fragment farther from the light than what the shadow map recorded?"* If yes → it's in shadow.

```
Pass 1: Light's POV
─────────────────────────────────────
Bind depthMapFBO
Set viewport to shadow map size (e.g. 1024x1024)
Render scene with simple depth shader
       → writes depth values into depthMap texture
Unbind FBO

         ↓  depthMap texture passed to Pass 2

Pass 2: Camera's POV
─────────────────────────────────────
Bind default framebuffer (screen)
Set viewport to screen size
Bind depthMap as a texture uniform
Render scene with full lighting shader
       → for each fragment, sample depthMap and
         compare depths to determine shadow
```

---

## 3. Setting Up the Depth Map (Pass 1)

### Framebuffer Object
You render into a texture instead of the screen using an FBO:

```cpp
GLuint depthMapFBO;
glGenFramebuffers(1, &depthMapFBO);
```

### Depth Texture
A single-channel `GL_DEPTH_COMPONENT` texture — stores one float per texel, no color:

```cpp
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
             SHADOW_WIDTH, SHADOW_HEIGHT, 0,
             GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
```

### Disable Color Buffer
Since you only need depth, explicitly tell OpenGL there's no color output:

```cpp
glDrawBuffer(GL_NONE);
glReadBuffer(GL_NONE);
```

---

## 4. The Light Space Matrix

To render from the light's perspective you need a view and projection matrix for the light, combined into a single **light space matrix**:

```cpp
glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);
glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
glm::mat4 lightSpaceMatrix = lightProjection * lightView;
```

- **Orthographic projection** is used for directional lights (parallel rays, no perspective distortion)
- **Perspective projection** would be used for spotlights or point lights
- The frustum bounds (`-10` to `10`, `near`, `far`) determine what area of the scene gets captured in the shadow map

This matrix transforms any world-space position into the light's clip space.

---

## 5. The Depth Shader (Pass 1)

A minimal shader — only transforms vertices, does nothing in the fragment shader:

```glsl
// Vertex shader
void main() {
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
}

// Fragment shader
void main() {
    // empty — GPU writes gl_FragDepth automatically
}
```

---

## 6. Shadow Calculation (Pass 2)

In the vertex shader, transform each fragment's world position into light space and pass it to the fragment shader:

```glsl
vs_out.frag_pos_light_space = lightSpaceMatrix * vec4(frag_pos, 1.0);
```

In the fragment shader:

```glsl
float calcShadow() {
    // 1. Perspective divide (no-op for orthographic, w = 1.0)
    vec3 projCoords = frag_pos_light_space.xyz / frag_pos_light_space.w;

    // 2. Remap from NDC [-1,1] to texture space [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    // 3. Sample the shadow map — gets closest depth the light saw
    float closestDepth = texture(depth_map, projCoords.xy).r;

    // 4. Current fragment's depth from light's perspective
    float currentDepth = projCoords.z;

    // 5. Compare — if fragment is deeper than recorded, it's in shadow
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    return shadow;
}
```

Apply shadow in the lighting calculation:

```glsl
vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
```

Ambient is always applied — shadow only blocks diffuse and specular.

---

## 7. Artifact: Shadow Acne

### What it looks like
Alternating lit/shadowed stripes across surfaces, even on surfaces that should be fully lit.

### Why it happens — two causes:

**Cause 1 — Floating point imprecision:**
The same surface measured in Pass 1 (shadow map) and Pass 2 (fragment shader) gives slightly different depth values due to floating point math. The surface shadows itself.

```
closestDepth = 0.5000  (recorded in Pass 1)
currentDepth = 0.5001  (measured in Pass 2)
0.5001 > 0.5000 → incorrectly flagged as shadow ❌
```

**Cause 2 — Light angle:**
When light hits a surface at a steep angle, a single shadow map texel covers a range of depths across the surface. Fragments at the far edge of a texel are genuinely deeper than what the texel recorded, causing self-shadowing even without floating point error.

The steeper the angle (light nearly parallel to surface), the larger the depth discrepancy per texel.

### The Fix — Shadow Bias

Subtract a small bias before comparing, adding a forgiveness margin:

```glsl
float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
```

**Adaptive bias** — scales with surface angle to handle both shallow and steep surfaces:

```glsl
float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
```

- `dot(normal, lightDir)` = 1.0 → head-on → small bias (0.005)
- `dot(normal, lightDir)` = 0.0 → grazing angle → large bias (0.05)

---

## 8. Artifact: Peter Panning

### What it looks like
Object shadows appear detached from the objects that cast them — objects look like they're floating.

### Why it happens
Bias that is too large pushes fragments that are genuinely in shadow (right at the base of an object) out of shadow range, making them appear lit. The shadow near the base disappears.

### The Fix — Front Face Culling

When rendering the depth map in Pass 1, cull front faces instead of back faces:

```cpp
glCullFace(GL_FRONT);
RenderSceneToDepthMap();
glCullFace(GL_BACK); // restore default
```

For solid closed objects, the back face depth is naturally slightly larger than the front face depth — giving you built-in bias without artificially shifting anything.

**Limitation:** Only works for solid closed objects. A flat plane has no back face — culling its front face removes it entirely from the depth map, causing it to never receive shadows.

---

## 9. Artifact: Oversampling

### What it looks like
Large dark regions outside the light's frustum coverage area appear to be in shadow when they shouldn't be.

### Why it happens
Fragments outside the light's frustum have `projCoords.xy` outside `[0, 1]`. With `GL_REPEAT` wrapping, out-of-range UV coordinates wrap around and sample random parts of the depth map, producing garbage depth values that incorrectly flag fragments as shadowed.

### Fix 1 — Clamp to Border

Set the shadow map's wrap mode to `GL_CLAMP_TO_BORDER` with a border color of `1.0`:

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
```

Any out-of-range UV sample now returns `closestDepth = 1.0` (maximum depth). Since no real fragment has depth greater than 1.0, they're never flagged as in shadow.

### Fix 2 — Clamp z Beyond Far Plane

`GL_CLAMP_TO_BORDER` only handles out-of-range `xy`. Fragments beyond the light's far plane have `projCoords.z > 1.0`, which the texture sampler never touches. Explicitly force these to be unshaded:

```glsl
if (projCoords.z > 1.0)
    shadow = 0.0;
```

---

## 10. Shadow Map Resolution vs. Frustum Size Tradeoff

The shadow map is always a fixed resolution (e.g. 1024×1024). The frustum size determines how much world-space area those texels are spread across:

```
Small frustum → texels cover a small area → high detail → sharp shadows
Large frustum → texels cover a large area → low detail → blocky shadows
```

Keep the frustum **just large enough** to contain everything that needs to cast or receive shadows. Anything outside the frustum will have no shadows — use `GL_CLAMP_TO_BORDER` and the `z > 1.0` check to ensure those regions appear correctly lit rather than incorrectly shadowed.

---

## 11. PCF — Percentage Closer Filtering (Soft Shadows)

### The Problem: Blocky Shadow Edges

The shadow map is a fixed resolution grid. Many screen fragments map to the same shadow map texel and get the same depth value back, meaning a whole block of fragments all get the same hard shadow answer. The boundary between shadow and light becomes a staircase of blocky squares.

### The Idea

Instead of sampling the depth map **once** and getting a hard 0 or 1, sample it **multiple times** at neighboring texels and **average** the results. This produces values between 0 and 1 at shadow edges — a smooth gradient instead of a hard cutoff.

### Implementation

The nested loop samples a 3×3 grid of 9 neighboring texels. `textureSize` gives the shadow map resolution, and dividing `1.0` by it gives the UV step size to move exactly one texel. Each sample gives a hard 0 or 1, all 9 are accumulated and divided by `9.0` to get an average. Fragments at the shadow boundary get a mix of shadowed and lit neighbors, producing the smooth gradient edge.

---

## 13. Orthographic vs Perspective Projection

Orthographic is used for directional lights (parallel rays, uniform shadow width). Perspective is used for spotlights/point lights (rays fan out, shadow widens with distance).

With perspective projection, depth is non-linearly compressed toward `1.0` — visualizing the depth map produces an almost entirely white image. `LinearizeDepth()` undoes the compression **for debugging only**. The actual shadow comparison never needs linearization because both values being compared are in the same non-linear space — only relative order matters, not actual distances.

---


## 14. Artifacts Quick Reference

| Artifact | Cause | Fix |
|---|---|---|
| **Shadow Acne** | Floating point imprecision + light angle causing self-shadowing | Shadow bias (adaptive based on surface angle) |
| **Peter Panning** | Bias too large, hides real shadows near object bases | Front face culling during depth map render |
| **Oversampling (xy)** | `GL_REPEAT` wraps out-of-frustum UV coordinates to wrong texels | `GL_CLAMP_TO_BORDER` with border color `1.0` |
| **Oversampling (z)** | Fragments beyond far plane have `z > 1.0`, comparison always true | Explicit `if (projCoords.z > 1.0) shadow = 0.0` |
| **Blocky shadows** | Shadow map resolution spread over too large a frustum | Reduce frustum size or increase shadow map resolution |