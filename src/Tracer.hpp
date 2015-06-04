#define __CL_ENABLE_EXCEPTIONS
#ifdef __APPLE__
#include <OpenCL/cl.h>
#include <OpenCL/cl_gl.h>
#include <OpenCL/cl_gl_ext.h>
#include <OpenCL/cl_platform.h>
#elif defined __linux__
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#include <CL/cl_platform.h>
#endif

#include <glad/glad.h>
#include <array>
#include <string>

#include "cl.hpp"

#include "Scene.hpp"
#include "Renderer.hpp"

class Tracer {
public:
    struct options {
        display_options dspo;
        bool shadows;

        bool operator!=(const options& o) {
            return dspo != o.dspo
                || shadows != o.shadows;
        }
    };

    Tracer(cl::Context, cl::Device, cl::CommandQueue);
    void load_kernels(options& options);
    void set_texture(GLuint texid, int width, int height);
    void set_scene(const Scene& scene);
    void set_options(options& options);
    void reload_kernels();
    void render();

private:
    cl::Context context;
    cl::Device device;
    cl::CommandQueue queue;

    const Scene* current_scene;

    int group_size;

    const std::string kernels_dir = "../src/kernels/";
    const std::array<std::string, 6> kernel_filenames = { { "tracer.cl",
                                                            "primitives.cl",
                                                            "intersect.cl",
                                                            "brdf.cl",
                                                            "shader.cl",
                                                            "quaternion.cl" } };

    cl::Program program;
    cl::Kernel tracer_krnl;

    cl::ImageGL target_texture;
    int width;
    int height;

    options current_options;

    void set_tracer_kernel_args();
};

void CL_CALLBACK contextCallback(const char*, const void*, size_t, void*);
