#ifndef BVH_H_
#define BVH_H_

#include "primitives.h"

uint expandBits(uint v);
uint morton3D(float3 v);

generateBVH(global int* sortedMortonCodes,
            global int* sortedObjectIDs,
            int numObjects);

#endif
