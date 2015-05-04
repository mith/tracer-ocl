
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

struct Plane {
    cl_float3 normal;
    cl_float offset;
    cl_int material;

    Plane(cl_float3 normal, cl_float offset, cl_int material)
        : normal(normal)
        , offset(offset)
        , material(material)
    {
    }
};

struct Sphere {
    cl_float3 center;
    cl_float radius;
    cl_int material;

    Sphere(cl_float3 center, cl_float radius, cl_int material)
        : center(center)
        , radius(radius)
        , material(material)
    {
    }
};

struct Light {
    cl_float3 color;
    cl_float3 location;
    cl_float radius;

    Light(cl_float3 color, cl_float3 location, cl_float radius)
        : color(color)
        , location(location)
        , radius(radius)
    {
    }
};

struct Triangle {
    cl_float3 a;
    cl_float3 b;
    cl_float3 c;
    cl_int material;

    Triangle(cl_float3 a, cl_float3 b, cl_float3 c, cl_int material)
        : a(a)
        , b(b)
        , c(c)
        , material(material)
    {
    }
};

struct Material {
    cl_float3 color;
    cl_float fresnel0;
    cl_float roughness;

    Material(cl_float3 color, cl_float fresnel0, cl_float roughness)
        : color(color)
        , fresnel0(fresnel0)
        , roughness(roughness)
    {
    }
};

struct Scene {
    std::array<Light, 1> lights = { { Light({ { 1.0 * M_PI, 1.0 * M_PI, 1.0 * M_PI } }, { { 0.0, 0.0, -70.0 } }, 120) } };

    std::array<Plane, 6> planes = { { Plane({ { 0.0, 1.0, 0.0 } }, 50, 4),
                                      Plane({ { 1.0, 0.0, 0.0 } }, 60, 1),
                                      Plane({ { 0.0, 0.0, 1.0 } }, 120, 2),
                                      Plane({ { -1.0, 0.0, 0.0 } }, 60, 3),
                                      Plane({ { 0.0, -1.0, 0.0 } }, 60, 0),
                                      Plane({ { 0.0, 0.0, -1.0 } }, -130, 5) } };

    std::array<Sphere, 3> spheres = { { Sphere({ { -30.0f, 30.0f, -90.0 } }, 19.0, 5),
                                        Sphere({ { -47.0, 10.0, -70.0 } }, 5.0, 0),
                                        Sphere({ { -30.0, 40.0, -100.0 } }, 5.0, 4) } };

    std::array<Triangle, 1> triangles = { { Triangle({ { 40.0, 0.0, -140.0 } }, { { 60.0, -20.0, -55.0 } }, { { 65.0, 30.0, -50.0 } }, 4) } };

    float fresnel0 = (1.0f - 1.5f) / (1.0f + 1.5f);

    std::array<Material, 6> materials = { { Material({ { 1.0 / M_PI, 0.0 / M_PI, 0.0 / M_PI } }, fresnel0 * fresnel0, 0.1),
                                            Material({ { 0.0 / M_PI, 1.0 / M_PI, 0.0 / M_PI } }, fresnel0 * fresnel0, 0.2),
                                            Material({ { 0.0 / M_PI, 0.0 / M_PI, 1.0 / M_PI } }, fresnel0 * fresnel0, 0.3),
                                            Material({ { 1.0 / M_PI, 1.0 / M_PI, 0.0 / M_PI } }, fresnel0 * fresnel0, 0.5),
                                            Material({ { 1.0 / M_PI, 1.0 / M_PI, 1.0 / M_PI } }, fresnel0 * fresnel0, 0.7),
                                            Material({ { 1.0 / M_PI, 0.5 / M_PI, 0.5 / M_PI } }, fresnel0 * fresnel0, 0.9) } };

    cl::Buffer lightsBuffer;
    cl::Buffer planesBuffer;
    cl::Buffer spheresBuffer;
    cl::Buffer trianglesBuffer;
    cl::Buffer materialsBuffer;

    cl::Context context;
    cl::Device device;
    cl::CommandQueue queue;

    Scene(cl::Context context, cl::Device device, cl::CommandQueue queue);
    void update();
};
