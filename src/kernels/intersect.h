///////
#ifndef INTERSECT_H_
#define INTERSECT_H_

float intersectPlane(struct Ray ray, struct Plane plane);
float intersectSphere(struct Ray ray, struct Sphere sphere);
float intersectTriangle(struct Ray ray, struct Triangle triangle);
float intersectAABB(struct Ray ray, struct AABB aabb);

#endif
