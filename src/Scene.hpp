#include <OpenCL/cl.h>

#include "cl.hpp"

#include <array>
#include <cmath>

struct Plane {
    cl_float3 normal;
    cl_float offset;
    cl_int material;
};

struct Sphere {
    cl_float3 center;
    cl_float radius;
    cl_int material;
};

struct Light {
    cl_float3 color;
    cl_float3 location;
    cl_float radius;
};

struct Triangle {
    cl_float3 points[3];
};
 
struct Material {
    cl_float3 color;
    cl_float fresnel0;
    cl_float roughness;
};

struct Scene
{
    std::array<Light, 1> lights = {
        Light({{1.0 * M_PI, 1.0 * M_PI, 1.0 * M_PI}, {-50.0, 0.0, -70.0}, 120})
    };

    std::array<Plane, 6> planes = {
        Plane({{0.0, 1.0, 0.0}, 50, 4}),
        Plane({{1.0, 0.0, 0.0}, 60, 1}),
        Plane({{0.0, 0.0, 1.0}, 120, 2}),
        Plane({{-1.0, 0.0, 0.0}, 60, 3}),
        Plane({{0.0, -1.0, 0.0}, 60, 0}),
        Plane({{0.0, 0.0, -1.0}, -130, 5})
    };
    
    std::array<Sphere, 3> spheres = {
        Sphere({{-30.0f, 30.0f, -90.0}, 19.0, 5}),
        Sphere({{-47.0, 10.0, -70.0}, 5.0, 0}),
        Sphere({{-30.0, 40.0, -100.0}, 5.0, 4}) 
    };

    std::array<Triangle, 1> triangles = {
        Triangle({{{0.0, 0.0, -30.0},{10.0, 0.0, -35.0},{10.0, 10.0, -30.0}}})
    };
    
    float fresnel0 = (1.0f - 1.5f) / (1.0f + 1.5f);

    std::array<Material, 6> materials = {
        Material({{1.0 / M_PI, 0.0 / M_PI, 0.0 / M_PI}, fresnel0 * fresnel0, 0.1}),
        Material({{0.0 / M_PI, 1.0 / M_PI, 0.0 / M_PI}, fresnel0 * fresnel0, 0.2}),
        Material({{0.0 / M_PI, 0.0 / M_PI, 1.0 / M_PI}, fresnel0 * fresnel0, 0.3}),
        Material({{1.0 / M_PI, 1.0 / M_PI, 0.0 / M_PI}, fresnel0 * fresnel0, 0.5}),
        Material({{1.0 / M_PI, 1.0 / M_PI, 1.0 / M_PI}, fresnel0 * fresnel0, 0.7}),
        Material({{1.0 / M_PI, 0.5 / M_PI, 0.5 / M_PI}, fresnel0 * fresnel0, 0.9})
    };

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

