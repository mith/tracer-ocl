#include "Scene.hpp"

#include "GLFW/glfw3.h"

#include <cmath>

Scene::Scene(cl::Context context, cl::Device device, cl::CommandQueue queue)
    : context(context), device(device), queue(queue)
{
    lightsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(Light) * lights.size());
    queue.enqueueWriteBuffer(lightsBuffer, CL_TRUE, 0, 
                             sizeof(Light) * lights.size(), &lights);

    planesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(Plane) * planes.size());
    queue.enqueueWriteBuffer(planesBuffer, CL_TRUE, 0, 
                             sizeof(Plane) * planes.size(), &planes);

    spheresBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(Sphere) * spheres.size());
    queue.enqueueWriteBuffer(spheresBuffer, CL_TRUE, 0, 
                             sizeof(Sphere) * spheres.size(), &spheres);

    materialsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, 
                                 sizeof(Material) * materials.size());
    queue.enqueueWriteBuffer(materialsBuffer, CL_TRUE, 0, 
                             sizeof(Material) * materials.size(), &materials);

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
}
