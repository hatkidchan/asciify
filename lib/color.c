#include "color.h"
#include "commons.h"

int color_compare(color_t a, color_t b)
{
    if (a.r != b.r || a.g != b.g || a.b != b.b)
        return 2;
    if (a.a != b.a)
        return 1;
    return 0;
}

int color_difference(color_t a, color_t b)
{
    int dr = a.r - b.r,
        dg = a.g - b.g,
        db = a.b - b.b;
    return sqrt(dr * dr + dg * dg + db * db);
}

float color_grayscale(color_t a)
{
    return 0.299 * a.r / 255.0 + 0.587 / 255.0 * a.g + 0.114 * a.b / 255.0;
}

bool color_bw(color_t a)
{
    return color_grayscale(a) >= 0.5;
}

uint8_t color_to_vt100(color_t c)
{
    if (c.r == c.g && c.g == c.b)
    {
        if (c.r < 8) return 16;
        if (c.r > 248) return 231;
        return 232 + ceil((c.r - 8.0) / 247.0 * 24.0);
    }
    uint8_t oc = 16;
    oc += 36 * ceil(c.r / 255.0 * 5.0);
    oc += 6  * ceil(c.g / 255.0 * 5.0);
    oc +=      ceil(c.b / 255.0 * 5.0);
    return oc;
}

color_t color_from_vt100(uint8_t c)
{
    color_t out = { 0, 0, 0, 255 };
    if (c >= 232)
    {
        int l = (c - 232) * 255 / 24;
        out.r = out.g = out.b = l;
    }
    else
    {
        c -= 16;
        out.b = (c % 6) * 42;
        c /= 6;
        out.g = (c % 6) * 42;
        c /= 6;
        out.r = (c % 6) * 42;
    }
    return out;
}

color_t color_clamp_vt100(color_t c)
{
    color_t out = color_from_vt100(color_to_vt100(c));
    out.a = c.a;
    return out;
}

color_t color_add(color_t a, color_t b)
{
    return (color_t){
        clamp(a.r + b.r, 0, 255),
        clamp(a.g + b.g, 0, 255),
        clamp(a.b + b.b, 0, 255),
        clamp(a.a + b.a, 0, 255)
    };
}

color_t color_sub(color_t a, color_t b)
{
    return (color_t){
        clamp(a.r - b.r, 0, 255),
        clamp(a.g - b.g, 0, 255),
        clamp(a.b - b.b, 0, 255),
        clamp(a.a - b.a, 0, 255)
    };
}

color_t color_mul(color_t a, color_t b)
{
    return (color_t){
        clamp(a.r * b.r / 255, 0, 255),
        clamp(a.g * b.g / 255, 0, 255),
        clamp(a.b * b.b / 255, 0, 255),
        clamp(a.a * b.a / 255, 0, 255)
    };
}

color_t color_mulF(color_t a, float f)
{
    return (color_t){
        a.r * f, a.g * f, a.b * f, a.a
    };
}

color_t color_closest(color_t a, color_t *pal, int len)
{
    float delta, closest_delta = 19075.f;
    color_t pc, closest = { 0, 0, 0, 0 };
    for (int i = 0; i < len; i++)
    {
        pc = pal[i];
        delta = color_difference(pc, a);
        if (delta < closest_delta)
        {
            closest_delta = delta;
            closest = pc;
        }
    }
    return closest;
}

