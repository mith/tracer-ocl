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

struct Scene {
    std::array<Light, 1> lights 
        = { { Light({ { 1.0 * M_PI, 1.0 * M_PI, 1.0 * M_PI } }, { { 0.0, 0.0, -70.0 } }, 120) } };

    std::array<Plane, 6> planes = { { Plane({ { 0.0, 1.0, 0.0 } }, 50, 4),
                                      Plane({ { 1.0, 0.0, 0.0 } }, 60, 1),
                                      Plane({ { 0.0, 0.0, 1.0 } }, 120, 2),
                                      Plane({ { -1.0, 0.0, 0.0 } }, 60, 3),
                                      Plane({ { 0.0, -1.0, 0.0 } }, 60, 0),
                                      Plane({ { 0.0, 0.0, -1.0 } }, -130, 5) } };

    std::array<Sphere, 3> spheres = { { Sphere({ { -30.0f, 30.0f, -90.0 } }, 19.0, 5),
                                        Sphere({ { -47.0, 10.0, -70.0 } }, 5.0, 0),
                                        Sphere({ { -30.0, 40.0, -100.0 } }, 5.0, 4) } };

    float fresnel0 = (1.0f - 1.5f) / (1.0f + 1.5f);

    std::array<Material, 6> materials 
        = { { Material({ { 1.0 / M_PI, 0.0 / M_PI, 0.0 / M_PI } }, fresnel0 * fresnel0, 0.1),
              Material({ { 0.0 / M_PI, 1.0 / M_PI, 0.0 / M_PI } }, fresnel0 * fresnel0, 0.2),
              Material({ { 0.0 / M_PI, 0.0 / M_PI, 1.0 / M_PI } }, fresnel0 * fresnel0, 0.3),
              Material({ { 1.0 / M_PI, 1.0 / M_PI, 0.0 / M_PI } }, fresnel0 * fresnel0, 0.5),
              Material({ { 1.0 / M_PI, 1.0 / M_PI, 1.0 / M_PI } }, fresnel0 * fresnel0, 0.7),
              Material({ { 1.0 / M_PI, 0.5 / M_PI, 0.5 / M_PI } }, fresnel0 * fresnel0, 0.9) } };

    cl::Buffer lightsBuffer;
    cl::Buffer planesBuffer;
    cl::Buffer spheresBuffer;
    cl::Buffer materialsBuffer;
    cl::Buffer vertexBuffer;
    cl::Buffer triangleBuffer;
    cl::Buffer meshesBuffer;

    cl::Context context;
    cl::Device device;
    cl::CommandQueue queue;

    Scene(cl::Context context, cl::Device device, cl::CommandQueue queue);
    void update();
};
