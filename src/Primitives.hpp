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

#include <vector>
#include <array>

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

struct Vertex {
    cl_float3 v;

    Vertex (std::array<float, 3> v)
        : v{{v[0], v[1], v[2]}} {}
};

struct Triangle {
    cl_uint3 i;

    Triangle (std::array<unsigned int, 3> i)
        : i{{i[0], i[1], i[2]}} {}
};

struct Mesh {
    std::vector<Vertex> positions;
    std::vector<Triangle> triangles;
};

struct CLMesh {
    cl_int num_vertices;
    cl_int material;

    cl_float3 translate;
    cl_float3 scale;
};

struct vt {
    std::array<float, 3> a;
};

struct id {
    std::array<unsigned int, 3> a;
};

