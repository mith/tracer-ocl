#include "Tracer.h"
#include <fstream>

#ifdef __linux__
#include <GL/glx.h>
#endif

Tracer::Tracer()
{
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);

    for (auto & platform : all_platforms) {
        std::cout << platform.getInfo<CL_PLATFORM_NAME>()
                  << std::endl;
    }

    cl::Platform default_platform = all_platforms[0];
    std::cout << "Using platform: "
              << default_platform.getInfo<CL_PLATFORM_NAME>()
              << std::endl;

    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

    for (auto & device : all_devices) {
        std::cout << device.getInfo<CL_DEVICE_NAME>()
                  << std::endl;
    }

#ifdef __APPLE__
    device = all_devices[2];
#elif defined __linux__
    device = all_devices[0];
#endif

    std::cout << "Using device: "
              << device.getInfo<CL_DEVICE_NAME>()
              << std::endl;

    cl_context_properties properties[] {
#ifdef __APPLE__
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
        (cl_context_properties) CGLGetShareGroup(CGLGetCurrentContext()),
#elif defined __linux__
        CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
        CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
        CL_CONTEXT_PLATFORM, (cl_context_properties)(all_platforms[0])(),
#endif
        0
    };

#ifdef __APPLE__
    context = cl::Context(device, properties, clLogMessagesToStdoutAPPLE);
#elif defined __linux__
    context = cl::Context(device, properties, &contextCallback);
#endif
    queue = cl::CommandQueue(context, device);

    scene = std::make_unique<Scene>(context, device, queue);
}

void CL_CALLBACK contextCallback(
        const char* errInfo,
        const void* private_info,
        size_t cb,
        void* user_data)
{
    std::cerr << errInfo << std::endl;
}

std::string file_str (std::string filename)
{
    std::fstream txt_file(filename);
    std::stringstream txt_buf;
    txt_buf.str("");
    txt_buf << txt_file.rdbuf();
    std::string src = txt_buf.str();
    return src;
}

void Tracer::load_kernels()
{
    cl::Program::Sources sources;

    std::vector<std::string> src_strs;

    for (auto & flnm : kernel_filenames) {
        src_strs.push_back(file_str(flnm));
    }

    for (auto & src : src_strs) {
        sources.push_back({src.c_str(), src.length()});
    }

    program = cl::Program(context, sources);
    if(program.build({device}, "-I ./kernels/") != CL_SUCCESS) {
        std::cerr << "error building: "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)
                  << std::endl;
    }
    tracer_krnl = cl::Kernel(program, "tracer");

    tracer_krnl.setArg(1, scene->lightsBuffer);
    tracer_krnl.setArg(2, scene->lights.size());

    tracer_krnl.setArg(3, scene->planesBuffer);
    tracer_krnl.setArg(4, scene->planes.size());

    tracer_krnl.setArg(5, scene->spheresBuffer);
    tracer_krnl.setArg(6, scene->spheres.size());

    tracer_krnl.setArg(7, scene->trianglesBuffer);
    tracer_krnl.setArg(8, scene->triangles.size());

    tracer_krnl.setArg(9, scene->materialsBuffer);
}

void Tracer::set_texture(GLuint texid, int width, int height)
{
    this->width = width;
    this->height = height;
    tex = cl::ImageGL(context,
                      CL_MEM_WRITE_ONLY,
                      GL_TEXTURE_2D,
                      0,
                      texid);
    tracer_krnl.setArg(0, tex);
}

void Tracer::trace()
{
    scene->update();

    std::vector<cl::Memory> mem_objs = {tex};
    glFlush();
    queue.enqueueAcquireGLObjects(&mem_objs, nullptr);
    queue.enqueueNDRangeKernel(tracer_krnl, cl::NullRange,
                                       cl::NDRange(width, height),
                                       cl::NDRange(32, 32),
                                       nullptr);
    queue.enqueueReleaseGLObjects(&mem_objs, nullptr);
    queue.finish();
}
