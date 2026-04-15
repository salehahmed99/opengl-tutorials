#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vendor/stb_image/stb_image.h"

GLuint loadShader(const std::string &path, GLenum shaderType);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

struct Color
{
    uint8_t r, g, b, a;
};

struct Vertex
{
    glm::vec3 position;
    Color color;
    glm::vec2 texCoord;
};

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // On Apple platforms, OpenGL core profile contexts
    // require forward compatibility  mode to be enabled.
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // load shaders, create and link shader program
    GLuint vs = loadShader("assets/shaders/simple.vert", GL_VERTEX_SHADER);
    GLuint fs = loadShader("assets/shaders/simple.frag", GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // remove shader objects once we've linked them into the program object
    glDeleteShader(vs);
    glDeleteShader(fs);

    Vertex vertices[] = {
        // positions          // colors               // texture coords
        {{0.5, 0.5, 0.0}, {255, 0, 0, 255}, {1.0, 1.0}},    // top right
        {{0.5, -0.5, 0.0}, {0, 255, 0, 255}, {1.0, 0.0}},   // bottom right
        {{-0.5, -0.5, 0.0}, {0, 0, 255, 255}, {0.0, 0.0}},  // bottom left
        {{-0.5, 0.5, 0.0}, {255, 255, 0, 255}, {0.0, 1.0}}, // top left
    };

    uint16_t indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, color)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, texCoord)));

    // load and create a texture
    // -------------------------
    GLuint texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // texture wrapping:
    // ------------------
    // GL_REPEAT (default) : Tiles the texture. UV 1.3 is treated the same as 0.3, UV 2.7 same as 0.7. Like infinitely tiling wallpaper.
    // GL_MIRRORED_REPEAT : Same as repeat but flips the texture every tile. UV 1.3 → 0.7, UV 2.3 → 0.7. Avoids visible seams at tile edges.
    // GL_CLAMP_TO_EDGE : Stretches the edge pixels outward. UV 1.5 just gives you the color of the rightmost/topmost pixel. No tiling at all.
    // GL_CLAMP_TO_BORDER : Anything outside 0–1 gets a solid color you specify. Good for shadow maps or UI elements where you want a hard boundary.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // texture filtering:
    // ------------------
    // GL_NEAREST (default) : just snaps to the closest texel and returns its color
    // GL_LINEAR : takes the 4 surrounding texels and blends them weighted by how close the UV is to each one

    // Mipmapping:
    // -------------------
    // GL_NEAREST_MIPMAP_NEAREST :
    // - Snaps to the closest mipmap level
    // - Uses nearest filtering within it
    // - Fully blocky, you can see hard jumps between mipmap levels

    // GL_LINEAR_MIPMAP_NEAREST :
    // - Snaps to the closest mipmap level
    // - Uses linear filtering within it
    // - Smooth within each level, but you still get visible "pop" when switching between levels

    // GL_NEAREST_MIPMAP_LINEAR :
    // - Linearly blends between the two closest mipmap levels
    // - Uses nearest filtering within each level
    // - Smooth level transitions but blocky within each level

    // GL_LINEAR_MIPMAP_LINEAR : (best quality, trilinear filtering)
    // - Linearly blends between the two closest mipmap levels
    // - Uses linear filtering within each level
    // - Fully smooth in every direction, no popping, no blockiness
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("assets/textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        // glTexImage2D parameters:
        // - target: the type of texture (1D, 2D, 3D, cube map, etc.)
        // - level: the mipmap level you're uploading. 0 = the base level (full resolution).
        //           If you were manually uploading mipmaps yourself you'd pass 1, 2, 3... for each smaller level.
        //          Usually you just pass 0 and let glGenerateMipmap handle the rest.
        // - internalFormat: the format you want OpenGL to store the texture in on the GPU. This is the internal format.
        // - width, height: dimensions of the texture, , which you got from your image loader (stb_image etc).
        // - border: must be 0 (legacy OpenGL allowed 1 for a border, but it's not supported in modern OpenGL)
        // - format: the format of the image data you're passing in from CPU memory. This tells OpenGL how to read your data buffer.
        //           This and the internal format are often the same, but don't have to be
        //           e.g. your image might have GL_RGBA (4 channels) but you only want to store GL_RGB (3 channels) on the GPU.
        // - type: the data type of each channel in your buffer. GL_UNSIGNED_BYTE means each R, G, B value is stored as a byte (0–255).
        // - data: pointer to the actual pixel data in memory
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 2
    // ---------
    GLuint texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("assets/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glUseProgram(program); // don't forget to activate/use the shader before setting uniforms!
    glUniform1i(glGetUniformLocation(program, "texture1"), 0); // sampler reads unit 0
    glUniform1i(glGetUniformLocation(program, "texture2"), 1); // sampler reads unit 1

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        float time = glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(VAO);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);           // "I'm now talking about slot 0"
        glBindTexture(GL_TEXTURE_2D, texture1); // "plug texture1 into slot 0"
        glActiveTexture(GL_TEXTURE1);           // "I'm now talking about slot 1"
        glBindTexture(GL_TEXTURE_2D, texture2); // "plug texture2 into slot 1"

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwDestroyWindow(window);
    glfwTerminate();
    glDeleteProgram(program);
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

GLuint loadShader(const std::string &path, GLenum shaderType)
{
    std::ifstream file(path);
    std::string source_code = std::string((std::istreambuf_iterator<char>(file)),
                                          std::istreambuf_iterator<char>());
    const char *source_code_str = source_code.c_str();

    GLuint shader = glCreateShader(shaderType);        // creates an empty shader object
    glShaderSource(shader, 1, &source_code_str, NULL); // attaches the GLSL source code string to the shader object
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetShaderInfoLog(shader, length, NULL, logStr);
        std::cout << "ERROR IN " << path << std::endl;
        std::cout << logStr << std::endl;
        delete[] logStr;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}