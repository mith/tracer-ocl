#ifndef INTERSECT_H_
#define INTERSECT_H_

float3 intersectTriangle(struct Ray ray, struct Triangle triangle);
float intersectAABB(struct Ray ray, struct AABB aabb);

#endif
