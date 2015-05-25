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

public:
    GLuint texture() { return tex; }

    Drawer(int width, int height);

    void display();
};


