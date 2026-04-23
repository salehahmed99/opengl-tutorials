## First, Understand What the Depth Map Actually Covers

The depth map is not infinite. It only captures what the light's camera could see within its frustum. Think of it like a photograph taken from the light's position — it only covers a limited rectangular area of the scene.

```
Light's frustum coverage projected onto the floor:

     ___________
    |           |
    |  depth    |
    |  map      |
    |  coverage |
    |___________|
    
Outside this box → not captured in the depth map at all
```

Anything outside this box has **no depth information recorded** for it.

---

## What Happens When You Sample Outside the Texture

When a fragment is outside the light's frustum, its `projCoords.xy` will be outside the `[0, 1]` range — maybe `1.3` or `-0.2`. You're trying to sample a texture at a coordinate that doesn't exist.

OpenGL doesn't crash — it handles out-of-range texture coordinates based on the **wrap mode** you set. You originally set:

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
```

`GL_REPEAT` means: if you go past `1.0`, just wrap around and start from `0.0` again. Like tiles on a floor — the same texture repeats infinitely.

So a fragment with `projCoords.xy = (1.3, 0.5)` actually samples the depth map at `(0.3, 0.5)` — a completely wrong location. That random depth value then gets compared against `currentDepth`, and often incorrectly flags the fragment as being in shadow.

This is why you see large shadow regions outside the light's actual coverage area — they're sampling garbage depth values due to texture repetition.

---

## The Fix: `GL_CLAMP_TO_BORDER`

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
```

`GL_CLAMP_TO_BORDER` means: if you sample outside `[0,1]`, return the **border color** instead of wrapping around.

You set the border color to `(1.0, 1.0, 1.0, 1.0)`, so any out-of-range sample returns `1.0`.

Now when an outside fragment samples the depth map:

```
closestDepth = 1.0   (border color, maximum possible depth)
currentDepth = 0.7   (some real fragment depth)

0.7 > 1.0? NO → not in shadow ✅
```

Since no real fragment can have a depth greater than `1.0` (that's the far plane), `currentDepth` will **always** be less than or equal to `closestDepth = 1.0`. So outside fragments are always considered lit — which is the correct behavior since the light simply has no information about those areas.

---

## Visual Summary

```
With GL_REPEAT:
     ___________
    |           |
    |  correct  |  ← depth map coverage, shadows work correctly
    |  shadows  |
    |___________|
////////////////////  ← outside: random repeated depth values → incorrect shadows everywhere


With GL_CLAMP_TO_BORDER (border = 1.0):
     ___________
    |           |
    |  correct  |  ← depth map coverage, shadows work correctly
    |  shadows  |
    |___________|
                    ← outside: always returns 1.0 → always lit, no false shadows
```

---

## One Sentence Summary

Fragments outside the light's frustum sample out-of-range texture coordinates. With `GL_REPEAT` they get random depth values causing false shadows. With `GL_CLAMP_TO_BORDER` set to `1.0`, they always get maximum depth, meaning they're never incorrectly shadowed.