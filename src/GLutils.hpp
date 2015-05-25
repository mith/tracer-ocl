#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

void gl_check_error(int line);
GLuint CreateShader(GLenum shaderType, const std::string& strShaderFile);
GLuint CreateProgram(const std::vector<GLuint>& shaderList);
