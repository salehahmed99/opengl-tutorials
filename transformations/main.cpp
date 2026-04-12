#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    GLuint mvp_loc = glGetUniformLocation(program, "MVP");

    Vertex vertices[] = {
        {{-0.5, -0.5, 0.0}, {255, 0, 0, 255}},  // bottom left
        {{0.5, -0.5, 0.0}, {0, 255, 0, 255}},   // bottom right
        {{0.5, 0.5, 0.0}, {0, 0, 255, 255}},    // top right
        {{-0.5, 0.5, 0.0}, {255, 255, 0, 255}}, // top left
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



    glEnable(GL_DEPTH_TEST); // enable depth testing for correct z-ordering
    glDepthFunc(GL_LESS); // specify the depth comparison function (fragments with less depth will be drawn in front of fragments with greater depth)
    glClearDepth(1.0f);  // specify the depth value used when the depth buffer is cleared (default is 1.0, which means farthest)
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 

    // glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    // glEnable(GL_SCISSOR_TEST);
    // glScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(VAO);

        glm::mat4 P = glm::perspective(glm::pi<float>() / 2.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
        glm::mat4 V = glm::lookAt(
            glm::vec3(2.0f * cosf(time), 1.0f, 2.0f * sinf(time)),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));

        for (int i = -2; i <= 2; i++){
            glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, (float)i));
            glm::mat4 MVP = P * V * M;
            glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &MVP[0][0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
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