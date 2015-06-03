#include <glad/glad.h>
#include "Scene.hpp"
#include "Renderer.hpp"

class Rasterizer
{
public:
    struct options {
        display_options dspo;
    };

    Rasterizer();

    void set_scene(const Scene& scene);
    void set_texture(GLuint texture, int width, int height);
    void set_options(options ro);
    void reload_shaders();
    void render();
private:
    GLuint target_texture;
    int width;
    int height;
    options current_options;

    const Scene* current_scene;
    const std::string shaders_dir = "../src/shaders/";
    const std::array<std::string, 2> shader_filenames = {{ "simple.vert",
                                                           "simple.frag" }};

    GLuint shader;
    GLuint positionBufferObject;
    GLuint attributesBufferObject;
    GLuint vao;
    GLuint framebuffer;
    GLuint color_renderbuffer;
    GLuint depth_renderbuffer;

    void load_shaders();
};
