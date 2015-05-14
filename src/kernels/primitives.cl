#include "primitives.h"

struct Triangle constructTriangle(global const struct Vertex* vertices,
                                  global const struct Indices* indices,
                                  int numTriangle,
                                  struct Mesh mesh)
{
    uint3 i = indices[numTriangle + mesh.base_triangle].vertex;
    struct Triangle triangle;
    triangle.a = vertices[i.x + mesh.base_vertex].position * mesh.scale + mesh.position;
    triangle.b = vertices[i.y + mesh.base_vertex].position * mesh.scale + mesh.position;
    triangle.c = vertices[i.z + mesh.base_vertex].position * mesh.scale + mesh.position;

    return triangle;
}


struct Ray createRay(float3 origin, float3 direction)
{
    struct Ray ray;
    ray.direction = direction;
    ray.origin = origin;
    ray.direction_inverse = 1.0f / direction;
    return ray;
}
