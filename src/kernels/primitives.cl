#include "primitives.h"

struct Triangle constructTriangle(global const struct Vertex* vertices,
                                  global const struct VertexAttributes* vertexAttributes,
                                  global const struct Indices* indices,
                                  int numTriangle,
                                  struct Mesh mesh)
{
    uint3 i = indices[numTriangle + mesh.base_triangle].vertex;
    struct Triangle triangle;
    triangle.a.position = rotate_quat(mesh.orientation, vertices[i.x + mesh.base_vertex].position) 
                        *  mesh.scale + mesh.position;
    triangle.b.position = rotate_quat(mesh.orientation, vertices[i.y + mesh.base_vertex].position) 
                        *  mesh.scale + mesh.position;
    triangle.c.position = rotate_quat(mesh.orientation, vertices[i.z + mesh.base_vertex].position) 
                        *  mesh.scale + mesh.position;

    triangle.aa = &vertexAttributes[i.x + mesh.base_vertex];
    triangle.ba = &vertexAttributes[i.y + mesh.base_vertex];
    triangle.ca = &vertexAttributes[i.z + mesh.base_vertex];

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
