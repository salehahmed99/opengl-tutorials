# OpenGL Clipping & NDC Space

## The Pipeline Order

Clipping happens **before** the perspective divide:

1. **Model space** → (model matrix) → **World space**
2. **World space** → (view matrix) → **View/Camera space**
3. **View space** → (projection matrix) → **Clip space** ← clipping happens here
4. **Clip space** → (perspective divide by w) → **NDC space**
5. **NDC space** → (viewport transform) → **Screen space**

---

## Clip Space & The Clip Test

After the projection matrix, vertices are in **clip space** with coordinates `(x, y, z, w)`. A vertex is inside the frustum if:

```
-w ≤ x ≤ w
-w ≤ y ≤ w
-w ≤ z ≤ w
```

`w` varies per vertex, so this is not a unit cube — it scales with depth. Geometry outside these bounds gets clipped, with new vertices interpolated on the frustum boundary.

---

## What is `w`?

After the projection matrix, `w` is the **negated camera-space Z depth**:

```
w_clip = -z_view
```

OpenGL looks down the **-Z axis**, so objects in front of the camera have negative `z_view`. For a vertex at `z_view = -5`:

```
w_clip = 5
```

The clip test then becomes `-5 ≤ x ≤ 5`, which reflects the fact that the frustum gets wider with depth.

`w` serves two purposes:
1. **Enables correct frustum clipping** before the divide
2. **Encodes depth** so the perspective divide produces correct perspective projection

---

## NDC Space

Only after clipping does the GPU perform the **perspective divide**:

```
NDC = (x/w, y/w, z/w)
```

Everything that survived clipping is now guaranteed to be in `[-1, 1]³`. NDC is the *result* of surviving clipping — not the input to it.

---

## Why Clipping Can't Happen in NDC

If you divided by `w` before clipping, a vertex behind the camera could have a negative `w`, causing a division by a negative number and flipping geometry inside-out. The clip-space inequalities avoid this by operating on the raw values before any divide.