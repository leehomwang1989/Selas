#pragma once

//==============================================================================
// Joe Schutte
//==============================================================================

#include <MathLib/FloatStructs.h>

namespace Shooty
{
    namespace Bsdf
    {
        float SmithGGXMasking(float3 wi, float3 wo, float3 wm, float a2);
        float SmithGGXMaskingShading(float3 wi, float3 wo, float3 wm, float a2);

        void GgxD(float roughness, float r0, float r1, float& theta, float& phi);
        float GgxDPdf(float roughness, float dotNH);        
        
        float3 GgxVndf(float3 wo, float roughness, float u1, float u2);
        float GgxVndfPdf(float roughness, float3 wo, float3 wm, float3 wi);
    }
}