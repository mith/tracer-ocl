#include "Drawer.hpp"
#include <cstdio>
#include <vector>

#include "imgui_impl_glfw_gl3.hpp"
#include <imgui.h>

#include "Utils.hpp"


void gl_check_error(int line)
{
    GLenum errorcode = glGetError();
    switch (errorcode) {
        case GL_INVALID_ENUM:
            std::printf("function called with invalid enum at line %d\n", line);
            break;
        case GL_INVALID_VALUE:
            std::printf("argument has invalid value at line %d\n", line);
            break;
        case GL_INVALID_OPERATION:
            std::printf("operation is invalid at line %d\n", line);
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::printf("framebuffer object not complete at line %d\n", line);
            break;
        default:
            std::printf("no error detected\n");
            break;
    }
}

GLuint CreateShader(GLenum shaderType, const std::string& strShaderFile)
{
    GLuint shader = glCreateShader(shaderType);
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
        switch (shaderType) {
            case GL_VERTEX_SHADER:
                strShaderType = "vertex";
                break;
            case GL_FRAGMENT_SHADER:
                strShaderType = "fragment";
                break;
        }

        std::fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType,
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

        auto strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, nullptr, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for (auto shader : shaderList) {
        glDetachShader(program, shader);
    }

    return program;
}

Drawer::Drawer(int width, int height)
{
    auto vertex_shader_src = file_to_str("../src/shaders/drawer.vert");
    auto fragment_shader_src = file_to_str("../src/shaders/drawer.frag");
    shader = CreateProgram({ CreateShader(GL_VERTEX_SHADER, vertex_shader_src),
            CreateShader(GL_FRAGMENT_SHADER, fragment_shader_src) });

    glGenBuffers(1, &positionBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4,
            (void*)(sizeof(GLfloat) * 2));

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

void Drawer::display()
{
    glBindVertexArray(vao);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glGenerateMipmap(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(shader, "tex"), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
