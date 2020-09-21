
// MT, 2017jan24

#include <stdlib.h>
#include <assert.h>

#include "Dim.h"

double Dim_getAspectRatio(struct Dim const * const inDim)
{
    assert(inDim!=NULL && inDim->h!=0);
    return (double)inDim->w/(double)inDim->h;
}

struct Dim Dim_getScaledInto(struct Dim const * const inOuter, struct Dim const * const inInner)
{
    assert(inOuter!=NULL && inInner!=NULL);
    
    struct Dim retVal;
    double const iAr = Dim_getAspectRatio(inInner);
    
    if(iAr>Dim_getAspectRatio(inOuter))
    {
        retVal.w = inOuter->w;
        retVal.h = (int)((double)retVal.w/iAr+0.5); // Rounds
    }
    else
    {
        retVal.h = inOuter->h;
        retVal.w = (int)((double)retVal.h*iAr+0.5); // Rounds
    }
    return retVal;
}