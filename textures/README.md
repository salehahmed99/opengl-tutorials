## The core idea

A texture has a fixed resolution, say **128×128 pixels**. But the geometry it's applied to can appear at **any size** on screen depending on how big you drew it or how far away it is in 3D.

So the question OpenGL asks is: **how many screen pixels does this surface occupy, vs how many texels (texture pixels) does the texture have?**


## The misconception

The 4 texture coordinates don't interpolate to produce the other texels. **The texels already exist — they're just the pixels of the image.** The texture is fully stored in GPU memory as-is.

What the 4 UV coordinates actually do is tell OpenGL **how to map the texture onto the geometry**. Then for each screen pixel (fragment) on that surface, OpenGL interpolates the UV coordinates and does a **lookup** into the texture — *"given this UV, what color is stored there?"*

So the flow is:


1. You load a 512x512 image → GPU stores all 512x512 texels in memory
2. Your 4 vertices each get a UV coordinate (e.g. the corners: 0,0  1,0  1,1  0,1)
3. For every **fragment** (screen pixel) covering the surface, the GPU interpolates a UV based on where that fragment sits between the vertices
3. That UV is handed to the **filter** (nearest/linear) which looks into the texture and returns a color
4. That color is what the fragment outputs to the screen

---

## What the filter actually does

The UV lands at some floating point position in the texture, like **(256.7, 133.2)**. It rarely lands exactly on a texel center. So the filter decides what color to return:

- **GL_NEAREST** — just snaps to the closest texel and returns its color
- **GL_LINEAR** — takes the 4 surrounding texels and blends them weighted by how close the UV is to each one


## Analogy

Think of the texture as a **pre-printed photo**. The 4 UV coordinates are like putting 4 pins on the corners of that photo and stretching it over your geometry. The photo's pixels were always there — the pins just determine how it gets stretched and aligned. The fragment shader then reads whatever pixel sits under each point of the surface.


## Magnification example

- 8×8 texture → 64 texels
- 16×16 screen area → 256 fragments
- Each texel covers a 2×2 block of screen pixels (256/64 = 4 pixels per texel)

For each of those 256 fragments, OpenGL interpolates a UV, then nearest filtering snaps to the closest texel center — so all 4 fragments in that 2×2 block get the **same color**. That's why nearest filtering looks **blocky/pixelated** during magnification — you can literally see the texel squares.

If you used **GL_LINEAR** instead, each fragment's UV would blend between the surrounding texels, so the transitions between texel colors would be smooth rather than hard blocks — that's why linear looks blurry during magnification.

This is exactly the classic tradeoff:
```
GL_NEAREST  →  sharp but blocky  (each 2x2 block is one flat color)
GL_LINEAR   →  smooth but blurry (colors blend across the 2x2 blocks)
```

## Minification example

- **512×512 texture** → 262,144 texels
- **16×16 screen area** → 256 fragments

So each fragment's interpolated UV covers a **32×32 block of texels** underneath it. OpenGL has to pick a color for that fragment from those 1024 texels it's sitting on top of.

---

### With nearest filtering

It just snaps to the single closest texel center and ignores the other 1023 texels under that fragment. This is the problem — as the surface moves or animates, the "closest" texel can suddenly jump to a completely different one frame to frame, causing **flickering and aliasing**.


### With GL_LINEAR filtering:

For each fragment, it takes the **4 closest texels** and blends them. But remember, each fragment covers a **32×32 block of texels** in your example — so GL_LINEAR is still only looking at 4 out of those 1024 texels and ignoring the rest.

This is better than GL_NEAREST (less flickering) but still fundamentally broken for heavy minification because:

- You're still throwing away most of the texture information
- The 4 texels it picks can still jump around frame to frame as the surface moves
- You still get aliasing artifacts, just slightly smoother ones than nearest

### The core problem

Neither GL_NEAREST nor GL_LINEAR solve minification properly because they both only sample a tiny number of texels, when really you need to **average a large region** of the texture to get an accurate color for that fragment.

Instead of trying to read from the full 512×512, OpenGL says *"this surface only has 16×16 pixels on screen, so I'll use the pre-generated 16×16 version of this texture"* — which was computed by properly averaging down all the texels. Each fragment then gets a smooth, accurate color with no flickering.

That's the whole point of mipmaps — rather than poorly sampling a huge texture for a tiny surface, you just **pre-shrink the texture** at load time into progressively smaller versions (512→256→128→64→...→1), and pick the right one at render time.

That's why the recommended setting is always:
```c
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```


The naming follows a simple pattern: **`GL_{between fragments}_{between mipmaps}`**

There are two decisions OpenGL has to make:

1. How to filter **within** the chosen mipmap level (nearest or linear)
2. How to transition **between** mipmap levels (nearest or linear)

---

## The 4 options

### `GL_NEAREST_MIPMAP_NEAREST`
- Snaps to the closest mipmap level
- Uses nearest filtering within it
- Fully blocky, you can see hard jumps between mipmap levels

### `GL_LINEAR_MIPMAP_NEAREST`
- Snaps to the closest mipmap level
- Uses linear filtering within it
- Smooth within each level, but you still get visible "pop" when switching between levels

### `GL_NEAREST_MIPMAP_LINEAR`
- Blends between the two closest mipmap levels
- Uses nearest filtering within each
- Smooth level transitions but blocky within each level

### `GL_LINEAR_MIPMAP_LINEAR` *(best quality, trilinear filtering)*
- Blends between the two closest mipmap levels
- Uses linear filtering within each level
- Fully smooth in every direction, no popping, no blockiness

---

The "popping" problem is what the second part solves. Without blending between mipmap levels, as an object moves away from the camera you can see a sudden jump the moment OpenGL switches from the 128×128 mipmap to the 64×64 one. Blending between them makes that transition invisible.