#include "Tracer.hpp"

#ifdef __linux__
#include <GL/glx.h>
#endif

#include <cmath>
#include <iostream>
#include "Utils.hpp"

Tracer::Tracer(cl::Context context, cl::Device device, cl::CommandQueue queue)
    : context(context)
    , device(device)
    , queue(queue)
    , current_scene(nullptr)
{
    auto max_group_size = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    group_size = std::sqrt(max_group_size);
}

void CL_CALLBACK contextCallback(
        const char* errInfo,
        const void* /*private_info*/,
        size_t /*cb*/,
        void* /*user_data*/)
{
    std::cerr << errInfo << std::endl;
}

void Tracer::load_kernels(Tracer::options & options)
{
    cl::Program::Sources sources;

    std::vector<std::string> src_strs;

    for (auto & flnm : kernel_filenames) {
        src_strs.push_back(file_to_str(kernels_dir + flnm));
    }

    for (auto & src : src_strs) {
        sources.push_back({src.c_str(), src.length()});
    }

    program = cl::Program(context, sources);
    std::string options_str("-DDISPLAY=");
    switch(options.display_options){
        case unlit:
            options_str.append("UNLIT");
            break;
        case normals:
            options_str.append("NORMALS");
            break;
        case texcoords:
            options_str.append("TEXCOORDS");
            break;
        case depth:
            options_str.append("DEPTH");
            break;
        case shaded:
        default:
            options_str.append("SHADED");
            break;
    }

    if(!options.shadows) {
        options_str.append(" -DNOSHADOWS");
    }
    try {
        program.build({device}, (options_str + " -cl-mad-enable -cl-std=CL1.2 -I " + kernels_dir).c_str());
    } catch (cl::Error err) {
        std::cerr << "error building: "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)
                  << std::endl;
    }
    tracer_krnl = cl::Kernel(program, "tracer");
}

void Tracer::set_scene(const Scene& scene)
{
    current_scene = &scene;
    set_tracer_kernel_args();
}

void Tracer::set_options(Tracer::options& options)
{
    if (options != current_options) {
        current_options = options;
        reload_kernels();
    }
}

void Tracer::reload_kernels()
{
    load_kernels(current_options);
    set_tracer_kernel_args();
    tracer_krnl.setArg(0, target_texture);
}

void Tracer::set_tracer_kernel_args()
{    
    tracer_krnl.setArg(1, current_scene->clview.lightsBuffer);
    tracer_krnl.setArg(2, (cl_int)current_scene->lights.size());

    tracer_krnl.setArg(3, current_scene->clview.vertexBuffer);
    tracer_krnl.setArg(4, current_scene->clview.vertexAttributesBuffer);
    tracer_krnl.setArg(5, current_scene->clview.indicesBuffer);
    tracer_krnl.setArg(6, current_scene->clview.meshesBuffer);
    tracer_krnl.setArg(7, (cl_int)current_scene->clmeshes.size());

    tracer_krnl.setArg(8, current_scene->clview.bvhBuffer);
    tracer_krnl.setArg(9, (cl_int)current_scene->bvh.size());

    tracer_krnl.setArg(10, current_scene->clview.materialsBuffer);
    tracer_krnl.setArg(11, current_scene->clview.diffuseBuffer);
}

void Tracer::set_texture(GLuint texid, int width, int height)
{
    try {
        this->width = width;
        this->height = height;
        target_texture = cl::ImageGL(context,
                          CL_MEM_WRITE_ONLY,
                          GL_TEXTURE_2D,
                          0,
                          texid);
        tracer_krnl.setArg(0, target_texture);
    } catch (cl::Error err) {
        std::cerr << "Error setting texture kernel arg, "
                  << err.what()
                  << std::endl;
        throw(err);
    }
}

void Tracer::render()
{
    std::vector<cl::Memory> mem_objs = {target_texture};
    glFlush();
    queue.enqueueAcquireGLObjects(&mem_objs, nullptr);
    queue.enqueueNDRangeKernel(tracer_krnl, cl::NullRange,
                                       cl::NDRange(width, height),
                                       cl::NDRange(group_size, group_size),
                                       nullptr);
    queue.enqueueReleaseGLObjects(&mem_objs, nullptr);
    queue.finish();
}
