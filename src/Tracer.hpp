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

#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>

#include "cl.hpp"

#include "Scene.hpp"

struct Ray {
    cl_float3 origin;
    cl_float3 direction;
};
class Tracer {
    cl::Context context;
    cl::Device device;
    cl::CommandQueue queue;

    const std::array<std::string, 4> kernel_filenames = { { "kernels/tracer.cl",
                                                            "kernels/intersect.cl",
                                                            "kernels/brdf.cl",
                                                            "kernels/shader.cl" } };

    cl::Program program;
    cl::Kernel tracer_krnl;

    cl::ImageGL tex;
    int width;
    int height;

    std::unique_ptr<Scene> scene;

public:
    Tracer();
    void load_kernels();
    void set_texture(GLuint texid, int width, int height);
    void trace();
};

void CL_CALLBACK contextCallback(const char*, const void*, size_t, void*);
