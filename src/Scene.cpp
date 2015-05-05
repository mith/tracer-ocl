#include "Scene.hpp"

#include "GLFW/glfw3.h"

#include <cmath>

Scene::Scene(cl::Context context, cl::Device device, cl::CommandQueue queue)
    : context(context)
    , device(std::move(device))
    , queue(queue)
{
    lightsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(lights));
    queue.enqueueWriteBuffer(lightsBuffer, CL_TRUE, 0,
                             sizeof(lights), &lights);

    planesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(planes));
    queue.enqueueWriteBuffer(planesBuffer, CL_TRUE, 0,
                             sizeof(planes), &planes);

    spheresBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(spheres));
    queue.enqueueWriteBuffer(spheresBuffer, CL_TRUE, 0,
                             sizeof(spheres), &spheres);

    trianglesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(spheres));
    queue.enqueueWriteBuffer(trianglesBuffer, CL_TRUE, 0,
                             sizeof(triangles), &triangles);

    materialsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY,
                                 sizeof(materials));
    queue.enqueueWriteBuffer(materialsBuffer, CL_TRUE, 0,
                             sizeof(materials), &materials);
}

void Scene::update()
{
    float x = fmod((glfwGetTime()), 2 * M_PI);
    spheres[0].center.s[0] = 0.f - std::sin(x - M_PI) * 30;
    spheres[0].center.s[1] = 0.0f - std::sin(x - M_PI / 2) * 30;
    spheres[1].center.s[0] = 0.f - std::sin(x - M_PI) * 40;
    queue.enqueueWriteBuffer(spheresBuffer, CL_TRUE, 0,
                             sizeof(Sphere) * spheres.size(), &spheres);

    lights[0].location.s[2] = -80.f - std::cos(x - M_PI) * 30;
    queue.enqueueWriteBuffer(lightsBuffer, CL_TRUE, 0,
                             sizeof(Light) * lights.size(), &lights);

    //triangles[0].a.s[2] = -100.0f - std::sin(x - M_PI) * 40;
    //queue.enqueueWriteBuffer(trianglesBuffer, CL_TRUE, 0,
    //                        sizeof(triangles), &triangles);
}
