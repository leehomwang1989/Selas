#pragma once

//==============================================================================
// Joe Schutte
//==============================================================================

//#include <MathLib/FloatStructs.h>
#include <systemLib/BasicTypes.h>

namespace Shooty
{
    struct float3;

    namespace ImportanceSampling
    {
        float BalanceHeuristic(uint nf, float fPdf, uint ng, float gPdf);
        float PowerHeuristic(uint nf, float fPdf, uint ng, float gPdf);
    }
}
