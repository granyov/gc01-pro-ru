/*
 * Rad Pro
 * STM32 compact math
 *
 * (C) 2022-2026 Gissio
 *
 * License: MIT
 */

#if defined(STM32)

#include <stdint.h>

#include "../system/cmath.h"

float log2f(float x)
{
    return 1.44269504088896F * logf(x);
}

float exp2f(float x)
{
    return expf(0.693147180559945F * x);
}

float log10f(float x)
{
    return 0.434294481903251F * logf(x);
}

float powf(float x, float y)
{
    return expf(y * logf(x));
}

#endif
