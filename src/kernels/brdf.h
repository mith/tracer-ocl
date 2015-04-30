#ifndef BRDF_H_
#define BRDF_H_

float distribution(float3 n, float3 h, float roughness);

float geometry(float3 n, float3 h, float3 v, float3 l, float roughnes);

float diffuse_energy_ratio(float f0, float3 n, float3 l);

float fresnel(float f0, float3 n, float3 l);

#endif
