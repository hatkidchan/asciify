#ifndef _COLOR_H
#define _COLOR_H
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} color_t;

int color_compare(color_t a, color_t b);
int color_difference(color_t a, color_t b);
float color_grayscale(color_t a);
bool color_bw(color_t a);
uint8_t color_to_vt100(color_t c);
color_t color_from_vt100(uint8_t c);
color_t color_clamp_vt100(color_t c);

#endif
