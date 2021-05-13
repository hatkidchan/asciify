#include "color.h"

int color_compare(struct color_t a, struct color_t b)
{
    if (a.r != b.r || a.g != b.g || a.b != b.b)
        return 2;
    if (a.a != b.a)
        return 1;
    return 0;
}

int color_difference(struct color_t a, struct color_t b)
{
    int dr = a.r - b.r,
        dg = a.g - b.g,
        db = a.b - b.b;
    return sqrt(dr * dr + dg * dg + db * db);
}

float color_grayscale(struct color_t a)
{
    return 0.299 * a.r / 255.0 + 0.587 / 255.0 * a.g + 0.114 * a.b / 255.0;
}

bool color_bw(struct color_t a)
{
    return color_grayscale(a) >= 0.5;
}

uint8_t color_to_vt100(struct color_t c)
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

struct color_t color_from_vt100(uint8_t c)
{
    struct color_t out = { 0, 0, 0, 255 };
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

struct color_t color_clamp_vt100(struct color_t c)
{
    struct color_t out = color_from_vt100(color_to_vt100(c));
    out.a = c.a;
    return out;
}
