//////

#include "brdf.h"

// Beckmann
float distribution(float3 n, float3 h, float roughness)
{
    float m_Sq = roughness * roughness;
    float NdotH_Sq = fmax(dot(n, h), 0.0f);
    NdotH_Sq = NdotH_Sq * NdotH_Sq;
    return exp( (NdotH_Sq - 1.0f) / (m_Sq * NdotH_Sq) )
    / (M_PI_F * m_Sq * NdotH_Sq * NdotH_Sq);
}

// CookTorrance
float geometry(float3 n, float3 h, float3 v, float3 l, float roughness)
{
    float NdotH = dot(n, h);
    float NdotL = dot(n, l);
    float NdotV = dot(n, v);
    float VdotH = dot(v, h);
    float NdotL_clamped = fmax(NdotL, 0.0f);
    float NdotV_clamped = fmax(NdotV, 0.0f);
    return min(min(2.0f * NdotH * NdotV_clamped / VdotH,
                   2.0f * NdotH * NdotL_clamped / VdotH),
               1.0f);
}

// FresnellDiff
float diffuse_energy_ratio(float f0, float3 n, float3 l)
{
    return 1.0f - fresnel(f0, n, l);
}

// Schlick
float fresnel(float f0, float3 n, float3 l)
{
    return f0 + (1.0f - f0) * pow(1.0f - dot(n, l), 5.0f);
}