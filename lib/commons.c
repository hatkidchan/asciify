#include "commons.h"

int clamp(int v, int a, int b)
{
    if (a > b) return clamp(v, b, a);
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

