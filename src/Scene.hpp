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

#include "cl.hpp"

#include <array>
#include <cmath>

#include "Primitives.hpp"

class Scene {
    cl::Context context;
    cl::Device device;
    cl::CommandQueue queue;

    Scene(cl::Context context, cl::Device device, cl::CommandQueue queue);

public:
    std::vector<Plane> planes;
    std::vector<Sphere> spheres;
    std::vector<Vertex> vertices;
    std::vector<VertexAttributes> vertexAttributes;
    std::vector<Indice> indices;
    std::vector<Mesh> meshes;
    std::vector<CLMesh> clmeshes;
    std::vector<BVHNode> bvh;
    std::vector<Light> lights;
    std::vector<Material> materials;

    cl::Buffer lightsBuffer;
    cl::Buffer planesBuffer;
    cl::Buffer spheresBuffer;
    cl::Buffer materialsBuffer;
    cl::Buffer vertexBuffer;
    cl::Buffer vertexAttributesBuffer;
    cl::Buffer indicesBuffer;
    cl::Buffer meshesBuffer;
    cl::Buffer bvhBuffer;

    static Scene load(const std::string & filename, cl::Context context, 
                cl::Device device, cl::CommandQueue queue);
    void update();
};
