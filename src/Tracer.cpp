#include "Tracer.hpp"
#include <fstream>
#include <boost/iostreams/device/mapped_file.hpp>

#ifdef __linux__
#include <GL/glx.h>
#endif

#include <cmath>

Tracer::Tracer(cl::Context context, cl::Device device, cl::CommandQueue queue)
    : context(context)
    , device(device)
    , queue(queue)
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

std::string file_to_str (std::string filename)
{
    using namespace boost::iostreams;
    mapped_file_source txt_file(filename);
    std::string src(txt_file.data(), txt_file.size());
    txt_file.close();
    return src;
}

void Tracer::load_kernels()
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
    try {
        program.build({device}, ("-I " + kernels_dir).c_str());
    } catch (cl::Error err) {
        std::cerr << "error building: "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)
                  << std::endl;
    }
    tracer_krnl = cl::Kernel(program, "tracer");

}

void Tracer::set_scene(const Scene& scene)
{
    tracer_krnl.setArg(1, scene.lightsBuffer);
    tracer_krnl.setArg(2, (cl_int)scene.lights.size());

    tracer_krnl.setArg(3, scene.planesBuffer);
    tracer_krnl.setArg(4, (cl_int)scene.planes.size());

    tracer_krnl.setArg(5, scene.spheresBuffer);
    tracer_krnl.setArg(6, (cl_int)scene.spheres.size());

    tracer_krnl.setArg(7, scene.vertexBuffer);
    tracer_krnl.setArg(8, scene.indicesBuffer);
    tracer_krnl.setArg(9, scene.meshesBuffer);
    tracer_krnl.setArg(10, (cl_int)scene.clmeshes.size());

    tracer_krnl.setArg(11, scene.bvhBuffer);
    tracer_krnl.setArg(12, (cl_int)scene.bvh.size());

    tracer_krnl.setArg(13, scene.materialsBuffer);
}

void Tracer::set_texture(GLuint texid, int width, int height)
{
    try {
        this->width = width;
        this->height = height;
        tex = cl::ImageGL(context,
                          CL_MEM_WRITE_ONLY,
                          GL_TEXTURE_2D,
                          0,
                          texid);
        tracer_krnl.setArg(0, tex);
    } catch (cl::Error err) {
        std::cerr << "Error setting texture kernel arg, "
                  << err.what()
                  << std::endl;
        throw(err);
    }
}

void Tracer::trace()
{
    std::vector<cl::Memory> mem_objs = {tex};
    glFlush();
    queue.enqueueAcquireGLObjects(&mem_objs, nullptr);
    queue.enqueueNDRangeKernel(tracer_krnl, cl::NullRange,
                                       cl::NDRange(width, height),
                                       cl::NDRange(group_size, group_size),
                                       nullptr);
    queue.enqueueReleaseGLObjects(&mem_objs, nullptr);
    queue.finish();
}
