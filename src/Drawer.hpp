#include <glad/glad.h>
#include <string>

class Drawer {
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

    const std::string vertexShaderSrc = 
        R"(#version 330
           layout(location = 0) in vec2 position;
           layout(location = 1) in vec2 texcoord;
           out vec2 f_texcoord;
           void main()
           {
               gl_Position = vec4(position, 0.0f, 1.0f);
               f_texcoord = texcoord;
           })";

    const std::string fragmentShaderSrc = ("#version 330\n"
                                           "in vec2 f_texcoord;\n"
                                           "out vec4 outputColor;\n"
                                           "uniform sampler2D tex;\n"
                                           "void main() {\n"
                                           "vec4 col = texture(tex, vec2(f_texcoord.x, -f_texcoord.y));\n"
                                           "outputColor = col;\n"
                                           "}\n");
public:
    GLuint texture() { return tex; }

    Drawer(int width, int height);

    void display();
};


