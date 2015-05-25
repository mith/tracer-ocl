#include "GLutils.hpp"

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


