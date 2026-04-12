#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>

GLuint loadShader(const std::string &path, GLenum shaderType);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

struct Position
{
    float x, y, z;
};
struct Color
{
    uint8_t r, g, b, a;
};

struct Vertex
{
    Position position;
    Color color;
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

    // The GPU has a table of uniform slots for each shader program
    // glGetUniformLocation gives you the index of the "time" row in that table
    // glUniform1f(time_loc, value) says "write this value into slot time_loc of the currently bound program"
    GLuint time_loc = glGetUniformLocation(program, "time");

    Vertex vertices[] = {
        {{-0.5, -0.5, 0.0}, {255, 0, 0, 255}},
        {{0.5, -0.5, 0.0}, {0, 255, 0, 255}},
        {{0.0, 0.5, 0.0}, {0, 0, 255, 255}}
    };

    // Asks OpenGL to create 1 buffer object and store its ID in VBO
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // "Selects" this buffer as the current active one.
    // GL_ARRAY_BUFFER means it's intended to hold vertex data.
    // Any subsequent buffer operations will target this buffer until you bind something else.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Actually uploads your CPU-side `vertices` array to GPU memory.
    // `GL_ARRAY_BUFFER` — target the currently bound array buffer.
    // `GL_STATIC_DRAW` — a hint telling OpenGL this data won't change often, so it can optimize its placement in GPU memory.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // A VAO (Vertex Array Object) is a container that remembers all
    // the "how to read the buffer" instructions below.
    // You create and bind it first so that everything that follows gets recorded inside it.
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Same idea as glGetUniformLocation but for input variables (attributes) in the vertex shader.
    // Gets the slot index of the position variable declared as in vec3 position; in your GLSL.
    GLuint position_loc = 0; // glGetAttribLocation(program, "position");

    // Enables that attribute slot. By default all slots are disabled, so you have to explicitly turn on the ones you're using.
    glEnableVertexAttribArray(position_loc);

    // The actual description of how to read the buffer. The arguments:
    // - `position_loc` — which attribute slot to configure
    // - `3` — each vertex has 3 values (x, y, z)
    // - `GL_FLOAT` — each value is a float
    // - `GL_FALSE` — don't normalize the values
    // - `sizeof(Vertex)` — **stride**: how many bytes to jump to get from one vertex to the next
    // - `0` — **offset**: start reading from byte 0
    glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    GLuint color_loc = 1; // glGetAttribLocation(program, "color");
    glEnableVertexAttribArray(color_loc);
    glVertexAttribPointer(color_loc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *) (sizeof(Position)));

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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(VAO);

        glUniform1f(time_loc, time);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
        // etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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