### The 4 Core Concepts

#### 1. Texture Objects (The Game Cartridges)
A Texture Object (`GLuint texture`) is the actual chunk of VRAM holding your image data (pixels, width, height, format). It is just storage. On its own, it does nothing until it is plugged into the system.

#### 2. Texture Units (The Console Slots)
Your GPU has a specific number of hardware slots designed to read textures simultaneously (usually at least 16 or 32). These are called **Texture Units** (`GL_TEXTURE0`, `GL_TEXTURE1`, etc.).
* You cannot send a Texture Object directly to a shader. It **must** be plugged into one of these Texture Units first.

#### 3. Active Texture (The Hand)
OpenGL is a state machine, meaning it only has one "hand" to manipulate things at a time. 
* **`glActiveTexture(GL_TEXTURE0);`** tells OpenGL: *"Move your hand to Slot 0."* * Any subsequent texture commands will now only affect Slot 0, until you move the hand again.

#### 4. Binding Textures (Plugging it in)
* **`glBindTexture(GL_TEXTURE_2D, myTexture);`** takes your Texture Object (the cartridge) and plugs it into whichever Texture Unit the "hand" is currently hovering over.

---

### The Fragment Shader Connection

The shader is completely isolated from your C++ code. It doesn't know what `myTexture` is. It only knows about the hardware slots.

* **`sampler2D` (The Cable):** In your GLSL code, `uniform sampler2D diffuseMap;` is essentially a cable waiting to be plugged into a Texture Unit.
* **Setting the Uniform:** When you write `shader.setInt("diffuseMap", 0);` in C++, you are telling the shader: *"Plug the `diffuseMap` cable into Texture Unit Index 0."*

*(Remember the gotcha: The state machine uses the macro `GL_TEXTURE0`, but the shader uniform just wants the raw integer `0`).*

---

### The Standard Workflow (Step-by-Step)

When you are ready to draw an object that uses multiple textures, the workflow always follows this exact sequence:

1.  **Select Slot 0:** `glActiveTexture(GL_TEXTURE0);`
2.  **Plug in Texture A:** `glBindTexture(GL_TEXTURE_2D, textureBase);`
3.  **Tell the Shader:** `shader.setInt("textureBase", 0);` *(Usually done once during setup)*
4.  **Select Slot 1:** `glActiveTexture(GL_TEXTURE1);`
5.  **Plug in Texture B:** `glBindTexture(GL_TEXTURE_2D, textureDecal);`
6.  **Tell the Shader:** `shader.setInt("textureDecal", 1);` *(Usually done once during setup)*
7.  **Draw the Object:** `glDrawArrays(...);`