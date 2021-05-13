#include "image_aspect.h"
#include <math.h>

void get_size_keep_aspect(int w, int h, int dw, int dh, int *ow, int *oh)
{
    (*ow) = dw;
    (*oh) = dh;
    float aspect = (float)w / (float)h;
    int tmp_1, tmp_2;
    if (((float)dw / (float)dh) >= aspect)
    {
        tmp_1 = floor(dh * aspect);
        tmp_2 = ceil(dh * aspect);
        if (fabs(aspect - tmp_1 / dh) < fabs(aspect - tmp_2 / dh))
            (*ow) = tmp_1 < 1 ? 1 : tmp_1;
        else
            (*ow) = tmp_2 < 1 ? 1 : tmp_2;
    }
    else
    {
        tmp_1 = floor(dw / aspect);
        tmp_2 = ceil(dw / aspect);
        if (tmp_2 == 0 || fabs(aspect - dw / tmp_1) < fabs(aspect - dw / tmp_2))
            (*oh) = tmp_1 < 1 ? 1 : tmp_1;
        else
            (*oh) = tmp_2 < 1 ? 1 : tmp_2;
    }
}

