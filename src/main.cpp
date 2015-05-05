#ifdef __APPLE__
#include <OpenCL/cl.h>
#include <OpenCL/cl_gl.h>
#include <OpenCL/cl_gl_ext.h>
#include <OpenCL/cl_platform.h>
#include <OpenGL/opengl.h>
#include <OpenGL/gl3.h>
#elif defined __linux__
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#include <CL/cl_platform.h>
#include "gl3.h"
#endif

#include <GLFW/glfw3.h>

#include "cl.hpp"
#include "Tracer.hpp"

void glfw_error(int error, const char* description)
{
    std::cerr << "Error " << error << ": " << description << std::endl;
}

void gl_check_error(int line)
{
    GLenum errorcode = glGetError();
    switch (errorcode) {
    case GL_INVALID_ENUM:
        printf("function called with invalid enum at line %d\n", line);
        break;
    case GL_INVALID_VALUE:
        printf("argument has invalid value at line %d\n", line);
        break;
    case GL_INVALID_OPERATION:
        printf("operation is invalid at line %d\n", line);
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        printf("framebuffer object not complete at line %d\n", line);
        break;
    default:
        printf("no error detected\n");
        break;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void window_resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile)
{
    GLuint shader = glCreateShader(eShaderType);
    const char* strFileData = strShaderFile.c_str();
    glShaderSource(shader, 1, &strFileData, nullptr);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto strInfoLog = std::vector<GLchar>(infoLogLength + 1);
        glGetShaderInfoLog(shader, infoLogLength, nullptr, strInfoLog.data());

        const char* strShaderType = nullptr;
        switch (eShaderType) {
        case GL_VERTEX_SHADER:
            strShaderType = "vertex";
            break;
        case GL_FRAGMENT_SHADER:
            strShaderType = "fragment";
            break;
        }

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType,
                strInfoLog.data());
    }

    return shader;
}

GLuint CreateProgram(const std::vector<GLuint>& shaderList)
{
    GLuint program = glCreateProgram();

    for (auto shader : shaderList) {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto  strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, nullptr, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for (auto shader : shaderList) {
        glDetachShader(program, shader);
    }

    return program;
}

struct Vertex {
    float position[2];
    float uvcoord[2];
};

struct FullscreenTextureDrawer {
    GLuint shader;
    GLuint positionBufferObject;
    GLuint elementBufferObject;
    GLuint vao;

    GLuint tex;

    const GLfloat vertices[16] = {
        // Position       Texcoords
        -1.0f, 1.0f, 0.0f, 0.0f, // Top-left
        1.0f, 1.0f, 1.0f, 0.0f, // Top-right
        1.0f, -1.0f, 1.0f, 1.0f, // Bottom-right
        -1.0f, -1.0f, 0.0f, 1.0f // Bottom-left
    };

    GLuint elements[6] = {
        0, 1, 2,
        2, 3, 0
    };

    const std::string vertexShaderSrc = ("#version 330\n"
                                         "layout(location = 0) in vec2 position;\n"
                                         "layout(location = 1) in vec2 texcoord;\n"
                                         "out vec2 f_texcoord;\n"
                                         "void main()\n"
                                         "{\n"
                                         "   gl_Position = vec4(position, 0.0f, 1.0f);\n"
                                         "   f_texcoord = texcoord;\n"
                                         "}\n");

    const std::string fragmentShaderSrc = ("#version 330\n"
                                           "in vec2 f_texcoord;\n"
                                           "out vec4 outputColor;\n"
                                           "uniform sampler2D tex;\n"
                                           "void main() {\n"
                                           "vec4 col = texture(tex, vec2(f_texcoord.x, -f_texcoord.y));\n"
                                           "outputColor = col;\n"
                                           "}\n");

    FullscreenTextureDrawer(int width, int height)
    {
        shader = CreateProgram({ CreateShader(GL_VERTEX_SHADER, vertexShaderSrc),
                                 CreateShader(GL_FRAGMENT_SHADER, fragmentShaderSrc) });

        glGenBuffers(1, &positionBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &elementBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenTextures(1, &tex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void display()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4,
                              (void*)(sizeof(GLfloat) * 2));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glGenerateMipmap(GL_TEXTURE_2D);
        glUniform1i(glGetUniformLocation(shader, "tex"), 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
};

int main()
{
    int width = 1024;
    int height = 512;
    glfwSetErrorCallback(glfw_error);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, "tracer", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_resize_callback);
    window_resize_callback(window, width, height);

    Tracer tracer;

    FullscreenTextureDrawer drawer(width, height);

    tracer.load_kernels();
    tracer.set_texture(drawer.tex, width, height);

    while (!glfwWindowShouldClose(window)) {
        tracer.trace();
        drawer.display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
