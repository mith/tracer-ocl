#pragma once

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

#include "cl.hpp"

#include <array>
#include <cmath>

#include "Primitives.hpp"

class Scene {
public:
    std::vector<Vertex> vertices;
    std::vector<VertexAttributes> vertexAttributes;
    std::vector<Indice> indices;
    std::vector<Mesh> meshes;
    std::vector<CLMesh> clmeshes;
    std::vector<BVHNode> bvh;
    std::vector<Light> lights;
    std::vector<Material> materials;
    std::vector<unsigned char> diffuse_array;

    struct GLView {
        GLuint vertexBuffer;
        GLuint vertexAttributesBuffer;
        GLuint indicesBuffer;
    };

    GLView glview;

    struct CLView {
        cl::Buffer lightsBuffer;
        cl::Buffer materialsBuffer;
        cl::Image2DArray diffuseBuffer;
        cl::Buffer vertexBuffer;
        cl::Buffer vertexAttributesBuffer;
        cl::Buffer indicesBuffer;
        cl::Buffer meshesBuffer;
        cl::Buffer bvhBuffer;
    };

    CLView clview;

    static Scene load(const std::string & filename, cl::Context context, 
                cl::Device device, cl::CommandQueue queue);
    void update();

private:
    cl::Context context;
    cl::Device device;
    cl::CommandQueue queue;

    Scene(cl::Context context, cl::Device device, cl::CommandQueue queue);
    void init_clview();
    void init_glview();
};

std::vector<unsigned char> load_texture(const std::string & filename);
