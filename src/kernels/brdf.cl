#include "brdf.h"

// Beckmann
float distribution (float3 n, float3 h, float roughness)
{
    float m_Sq = roughness * roughness;
    float NdotH_Sq = fmax(dot(n, h), 0.0f);
    NdotH_Sq = NdotH_Sq * NdotH_Sq;
    return exp( (NdotH_Sq - 1.0f) / (m_Sq * NdotH_Sq) ) 
           / (M_PI_F * m_Sq * NdotH_Sq * NdotH_Sq);
}

// BlinnPhong
//float distribution (float3 n, float3 h, float roughness)
//{
//    float m = 2.0f / (roughness * roughness) - 2.0f;
//    return (m + 2.0f) * pow(fmax(dot(n, h), 0.0f), m) 
//           / (2.0f * M_PI_F);
//}

// Implicit
//float geometry(float3 n, float3 h, 
//                        float3 v, float3 l, float roughness)
//{
//    return fmax(dot(n, l), 0.0f) * fmax(dot(n, v), 0.0f);
//}

// CookTorrance
float geometry(float3 n, float3 h, 
                            float3 v, float3 l, float roughness)
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

// Schlick
// float geometry(float3 n, float3 h, 
//                        float3 v, float3 l, float roughness)
// {
//     float NdotL_clamped = fmax(dot(n, l), 0.0f);
//     float NdotV_clamped = fmax(dot(n, v), 0.0f);
//     float k = roughness * sqrt(2.0f / M_PI_F);
//     float one_minus_k = 1.0f - k;
//     return (NdotL_clamped / (NdotL_clamped * one_minus_k + k))
//          * (NdotV_clamped / (NdotV_clamped * one_minus_k + k));
// }

// Walter
// float geometry(float3 n, float3 h, 
//                       float3 v, float3 l, float roughness)
// {
//     float NdotV = dot(n, v);
//     float NdotL = dot(n, l);
//     float HdotV = dot(h, v);
//     float HdotL = dot(h, l);
//     float NdotV_clamped = fmax(NdotV, 0.0f);
//     float a = 1.0f / (roughness * tan(acos(NdotV_clamped)));
//     float a_Sq = a * a;
//     float a_term;
//     if (a < 1.6f)
//         a_term = (3.535f * a + 2.181f * a_Sq) / (1.0f + 2.276 * a + 2.577 * a_Sq);
//     else
//         a_term = 1.0f;
//     return (step(0.0f, HdotL / NdotL) * a_term)
//          * (step(0.0f, HdotV / NdotV) * a_term);
// }

// None
// float diffuse_energy_ratio(float f0, float3 n, float3 l)
// {
//     return 1.0f;
// }

// FresnelDiff
float diffuse_energy_ratio(float f0, float3 n, float3 l)
{
    return 1.0f - fresnel(f0, n, l);
}

// Fresnel0
// float diffuse_energy_ratio(float f0, float3 n, float3 l)
// {
//     return 1.0f - f0;
// }

// Schlick
float fresnel(float f0, float3 n, float3 l)
{
    return f0 + (1.0f - f0) * pow(1.0f - dot(n, l), 5.0f);
}
