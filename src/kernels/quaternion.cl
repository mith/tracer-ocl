#include "quaternion.h"

float3 rotate_quat(quaternion q, float3 v)
{
    //float3 t = 2 * cross(q.xyz, v);
    //return v + q.w * t + cross(q.xyz, t);
    float3 u = q.xyz;
    float s = q.w;

    return 2.0f * dot(u, v) * u
         + (s * s - dot(u, u)) * v
         + 2.0f * s * cross(u, v);
}
