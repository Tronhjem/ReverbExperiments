//
//  UtilityFunctions.h
//  Created by Christian Tronhjem on 15.01.22.
//

#ifndef UtilityFunctions_h
#define UtilityFunctions_h
#include "SonoTronTypes.h"


namespace MyDSP
{
    unsigned int inline NearestPowerOfTwo(const int size)
    {
        int exponent = ceil(log(size)/log(2));
        return pow(2, exponent);
    }

    double inline Lerp(const double a, const double b, const double t)
    {
        return (1 - t) * a + t * b;
    }
}


#endif /* UtilityFunctions_h */


