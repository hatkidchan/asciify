#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include "stb_image.h"
#include "stb_image_resize.h"
#include "color.h"
#include "commons.h"
#include "image_aspect.h"

enum { MODE_BW = 0, MODE_256 = 1, MODE_TRUECOLOR = 2 };
enum { OMODE_TERMINAL = 0, OMODE_HTML = 1 };

void usage(char *progname)
{
    fprintf(stderr, "usage: %s ", progname);
    fprintf(stderr, "[-hvTM] [-o FILE] [-W WIDTH] [-H HEIGHT] FILE\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "-h\t\tShow this help\n");
    fprintf(stderr, "-v\t\tBe verbose\n");
    fprintf(stderr, "-T\t\tUse TrueColor\n");
    fprintf(stderr, "-M\t\tOutput as HTML\n");
    fprintf(stderr, "-o FILE\t\tOutput filename (- or stdout by default)\n");
    fprintf(stderr, "-W WIDTH\tResult width (in characters)\n");
    fprintf(stderr, "-H HEIGHT\tResult height (in characters)\n");
    fprintf(stderr, "FILE\t\tInput image\n");
}

int best_match_i(color_t a, color_t b, color_t t)
{
    return color_difference(a, t) < color_difference(b, t) ? 0 : 1;
}

int main(int argc, char **argv)
{
    int in_w, in_h, out_w = 80, out_h = 24, res_w = out_w, res_h = out_h, n_ch;
    char *in_filename = NULL,
         *out_filename = NULL;

    bool verbose = false;
    int mode = MODE_256, out_mode = OMODE_TERMINAL;

    FILE *out_fh = stdout;

    int c;
    while ((c = getopt(argc, argv, "vMGTho:W:H:")) != -1)
    {
        switch (c)
        {
            case 'T':
                mode = MODE_TRUECOLOR;
                break;
            case 'G':
                mode = MODE_BW;
                break;
            case 'M':
                out_mode = OMODE_HTML;
                break;
            case 'v':
                verbose = true;
                break;
            case 'o':
                out_filename = optarg;
                break;
            case 'W':
                if ((out_w = atoi(optarg)) < 1)
                {
                    fprintf(stderr, "Error: Width is invalid\n");
                    return 2;
                }
                break;
            case 'H':
                if ((out_h = atoi(optarg)) < 1)
                {
                    fprintf(stderr, "Error: Height is invalid\n");
                    return 2;
                }
                break;
            case 'h':
                usage(argv[0]);
                return 0;
                break;
            case '?':
                if (optopt == 'o' || optopt == 'W' || optopt == 'H')
                    fprintf(stderr,
                            "Error: Parameter -%c requires an argument\n",
                            optopt);
                else
                    fprintf(stderr, "Error: Unknown parameter %c\n", optopt);
                usage(argv[0]);
                return 1;
                break;
        }
    }
    
    if (argc <= optind || argc < 2)
    {
        fprintf(stderr, "Error: No image provided\n");
        usage(argv[0]);
        return 3;
    }
    
    in_filename = argv[optind];

    color_t *source_im, *scaled_im, pixs[8], color_min, color_max;
    int dist, dist_min, dist_max;
    source_im = (color_t *) stbi_load(in_filename, 
            &in_w, &in_h, &n_ch, STBI_rgb_alpha);
    
    DBGF("Source image size: %d:%d\n", in_w, in_h);
    DBG("Image is outside of specified size, resizing to ");
    get_size_keep_aspect(in_w, in_h, out_w * 2, out_h * 4, &res_w, &res_h);
    DBGF("%d:%d\n", res_w, res_h);
    DBGF("Allocating %ld bytes for resized image ...",
            res_w * res_h * sizeof(color_t));
    scaled_im = calloc(res_w * res_h, sizeof(color_t));
    DBGF("%p\n", (void *) scaled_im);
    stbir_resize_uint8((uint8_t *) source_im,  in_w,  in_h, 0,
                       (uint8_t *) scaled_im, res_w, res_h, 0, 4);

    DBGF("Final image dimensions: %d:%d\n", res_w, res_h);

    if (out_filename != NULL)
    {
        DBGF("Opening %s for writing\n", out_filename);
        out_fh = fopen(out_filename, "w");
        if (out_fh == NULL)
        {
            perror("fopen()");
            return 4;
        }
    }
    
    DBG("Starting processing...\n");

    int charcode;
    uint8_t braille_char[4];
    for (int y = 0; y < res_h; y += 4)
    {
        for (int x = 0; x < res_w; x += 2)
        {
            pixs[0] = scaled_im[(x + 0) + (y + 0) * res_w];
            pixs[3] = scaled_im[(x + 1) + (y + 0) * res_w];
            pixs[1] = scaled_im[(x + 0) + (y + 1) * res_w];
            pixs[4] = scaled_im[(x + 1) + (y + 1) * res_w];
            pixs[2] = scaled_im[(x + 0) + (y + 2) * res_w];
            pixs[5] = scaled_im[(x + 1) + (y + 2) * res_w];
            pixs[6] = scaled_im[(x + 0) + (y + 3) * res_w];
            pixs[7] = scaled_im[(x + 1) + (y + 3) * res_w];
            if (mode == MODE_256)
                for (int i = 0; i < 8; i++)
                    pixs[i] = color_clamp_vt100(pixs[i]);
            dist_min = 0xfd02ff; // 255 cubed
            dist_max = 0;
            
            color_max = scaled_im[x + y * res_w];
            color_min = color_max;

            for (int i = 0; i < 8; i++)
            {
                dist = color_difference(pixs[i], (color_t){0, 0, 0, 255});
                if (dist < dist_min)
                {
                    dist_min = dist;
                    color_min = pixs[i];
                }
                
                if (dist > dist_max)
                {
                    dist_max = dist;
                    color_max = pixs[i];
                }
            }
            
            charcode = 0x2800;
            for (int i = 0; i < 8; i++)
            {
                if (best_match_i(color_min, color_max, pixs[i]) == 1)
                    charcode |= (1 << i);
            }
            
            fprintf(out_fh, SEQ_TRUECOLOR_BOTH,
                    color_min.r, color_min.g, color_min.b,
                    color_max.r, color_max.g, color_max.b);
            
            braille_char[0] = 0xe2;
            braille_char[1] = 0x80 | ((charcode >> 6) & 0x3f);
            braille_char[2] = 0x80 | ((charcode >> 0) & 0x3f);
            braille_char[3] = 0x00;
            
            fprintf(out_fh, "%s", braille_char);
            
        }
        fprintf(out_fh, "\x1b[0m\n");
    }

    DBG("Job done, cleaning up...\n");
    if (out_fh != NULL && out_fh != stdout)
    {
        DBG("Closing file\n");
        fclose(out_fh);
    }
    if (source_im != scaled_im)
    {
        DBG("Freeing memory with scaled version\n");
        stbi_image_free(scaled_im);
    }
    DBG("Freeing memory with source image\n");
    stbi_image_free(source_im);
}
