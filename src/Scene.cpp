#include "Scene.hpp"
#include "Meshloader.hpp"

#include "GLFW/glfw3.h"

#include <cmath>

Scene::Scene(cl::Context context, cl::Device device, cl::CommandQueue queue)
    : context(context)
    , device(std::move(device))
    , queue(queue)
{
    lightsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(lights));
    queue.enqueueWriteBuffer(lightsBuffer, CL_FALSE, 0,
                             sizeof(lights), &lights);

    planesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(planes));
    queue.enqueueWriteBuffer(planesBuffer, CL_FALSE, 0,
                             sizeof(planes), &planes);

    spheresBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(spheres));
    queue.enqueueWriteBuffer(spheresBuffer, CL_FALSE, 0,
                             sizeof(spheres), &spheres);

    materialsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY,
                                 sizeof(materials));
    queue.enqueueWriteBuffer(materialsBuffer, CL_FALSE, 0,
                             sizeof(materials), &materials);

    Mesh tetra = load_mesh("../meshes/tetrahedron.iqm");

    vertexBuffer = cl::Buffer(context, CL_MEM_READ_ONLY,
                              sizeof(Vertex) * tetra.positions.size());
    queue.enqueueWriteBuffer(vertexBuffer, CL_FALSE, 0,
                             sizeof(Vertex) * tetra.positions.size(),
                             tetra.positions.data());

    triangleBuffer = cl::Buffer(context, CL_MEM_READ_ONLY,
                                sizeof(Triangle) * tetra.triangles.size());
    queue.enqueueWriteBuffer(triangleBuffer, CL_FALSE, 0,
                             sizeof(Triangle) * tetra.triangles.size(),
                             tetra.triangles.data());

    CLMesh cm({(cl_int)tetra.triangles.size(), 
               1,
               {{-30.0f, 10.0f, -80.0f}},
               {{2.0f, 2.0f, 2.0f}}});
    meshesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY,
                              sizeof(CLMesh)); 
    queue.enqueueWriteBuffer(meshesBuffer, CL_TRUE, 0,
                             sizeof(CLMesh), &cm);
}

void Scene::update()
{
    float x = fmod((glfwGetTime()), 2 * M_PI);
    spheres[0].center.s[0] = 0.f - std::sin(x - M_PI) * 30;
    spheres[0].center.s[1] = 0.0f - std::sin(x - M_PI / 2) * 30;
    spheres[1].center.s[0] = 0.f - std::sin(x - M_PI) * 40;
    queue.enqueueWriteBuffer(spheresBuffer, CL_FALSE, 0,
                             sizeof(Sphere) * spheres.size(), &spheres);

    lights[0].location.s[2] = -80.f - std::cos(x - M_PI) * 30;
    queue.enqueueWriteBuffer(lightsBuffer, CL_FALSE, 0,
                             sizeof(Light) * lights.size(), &lights);
}
