#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include "stb_image.h"
#include "stb_image_resize.h"
#include "color.h"
#include "commons.h"
#include "image_aspect.h"

#define BLOCK_TOP "\u2580"
#define BLOCK_BOT "\u2584"
#define BLOCK_FUL "\u2588"
#define BLOCK_NUL " "

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

    color_t *source_im, *scaled_im, pix_top, pix_bot;
    source_im = (color_t *) stbi_load(in_filename, 
            &in_w, &in_h, &n_ch, STBI_rgb_alpha);
    
    DBGF("Source image size: %d:%d\n", in_w, in_h);
    if (in_w <= out_w && in_h <= out_h * 2)
    {
        DBG("Image in bounds of output, keeping as it is\n");
        scaled_im = source_im;
        res_w = in_w;
        res_h = in_h;
    }
    else
    {
        DBG("Image is outside of specified size, resizing to ");
        get_size_keep_aspect(in_w, in_h, out_w, out_h * 2, &res_w, &res_h);
        DBGF("%d:%d\n", res_w, res_h);
        DBGF("Allocating %ld bytes for resized image ...",
             res_w * res_h * sizeof(color_t));
        scaled_im = calloc(res_w * res_h, sizeof(color_t));
        DBGF("%p\n", (void *) scaled_im);
        stbir_resize_uint8((uint8_t *) source_im,  in_w,  in_h, 0,
                           (uint8_t *) scaled_im, res_w, res_h, 0, 4);
    }

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

    if (out_mode == OMODE_HTML)
        fprintf(out_fh, SEQ_HTML_BEGIN);

    for (int y = 0; y < res_h - 1; y += 2)
    {
        int ci_top, ci_bot;
        bool top, bot;
        for (int x = 0; x < res_w; x++)
        {
            pix_top = scaled_im[x + res_w * y];
            pix_bot = scaled_im[x + res_w * (y + 1)];
            ci_top = color_to_vt100(pix_top);
            ci_bot = color_to_vt100(pix_bot);
            top = mode == MODE_BW ? color_bw(pix_top) : pix_top.a >= 127;
            bot = mode == MODE_BW ? color_bw(pix_bot) : pix_bot.a >= 127;
            if (mode == MODE_256)
            {
                pix_top = color_clamp_vt100(pix_top);
                pix_bot = color_clamp_vt100(pix_bot);
            }

            switch (mode)
            {
                case MODE_256:
                    if (!top && !bot)
                    { /**/ }
                    else if (top && !bot && out_mode == OMODE_TERMINAL)
                        fprintf(out_fh, SEQ_VT100_FG, ci_top);
                    else if (top && !bot && out_mode == OMODE_HTML)
                        fprintf(out_fh, SEQ_HTML_FG,
                                pix_top.r, pix_top.g, pix_top.b);
                    else if (!top && bot && out_mode == OMODE_TERMINAL)
                        fprintf(out_fh, SEQ_VT100_FG, ci_bot);
                    else if (!top && bot && out_mode == OMODE_HTML)
                        fprintf(out_fh, SEQ_HTML_FG,
                                pix_bot.r, pix_bot.g, pix_bot.b);
                    else if (out_mode == OMODE_TERMINAL)
                        fprintf(out_fh, SEQ_VT100_BOTH, ci_top, ci_bot);
                    else if (out_mode == OMODE_HTML)
                        fprintf(out_fh, SEQ_HTML_BOTH,
                                pix_top.r, pix_top.g, pix_top.b,
                                pix_bot.r, pix_bot.g, pix_bot.b);
                    break;

                case MODE_TRUECOLOR:
                    if (!top && !bot)
                    { /**/ }
                    else if (top && !bot && out_mode == OMODE_TERMINAL)
                        fprintf(out_fh, SEQ_TRUECOLOR_FG,
                                pix_top.r, pix_top.g, pix_top.b);
                    else if (top && !bot && out_mode == OMODE_HTML)
                        fprintf(out_fh, SEQ_HTML_FG,
                                pix_top.r, pix_top.g, pix_top.b);
                    else if (!top && bot && out_mode == OMODE_TERMINAL)
                        fprintf(out_fh, SEQ_TRUECOLOR_FG,
                                pix_top.r, pix_top.g, pix_top.b);
                    else if (!top && bot && out_mode == OMODE_HTML)
                        fprintf(out_fh, SEQ_HTML_FG,
                                pix_bot.r, pix_bot.g, pix_bot.b);
                    else if (out_mode == OMODE_TERMINAL)
                        fprintf(out_fh, SEQ_TRUECOLOR_BOTH,
                                pix_top.r, pix_top.g, pix_top.b,
                                pix_bot.r, pix_bot.g, pix_bot.b);
                    else if (out_mode == OMODE_HTML)
                        fprintf(out_fh, SEQ_HTML_BOTH,
                                pix_top.r, pix_top.g, pix_top.b,
                                pix_bot.r, pix_bot.g, pix_bot.b);
                    break;

                default:
                    break;
            }
            if (!top && !bot)
                fprintf(out_fh, BLOCK_NUL);
            else if (top && !bot)
                fprintf(out_fh, BLOCK_TOP);
            else if (!top && bot)
                fprintf(out_fh, BLOCK_BOT);
            else
                fprintf(out_fh, mode == MODE_BW ? BLOCK_FUL : BLOCK_BOT);
            switch (mode)
            {
                case MODE_TRUECOLOR:
                case MODE_256:
                    if (!top && !bot)
                    { /**/ }
                    else if ((top ^ bot) && out_mode == OMODE_HTML)
                        fprintf(out_fh, SEQ_HTML_CLOSE);
                    else if ((top && bot) && out_mode == OMODE_HTML)
                        fprintf(out_fh, "%s", SEQ_HTML_CLOSE);
                    else if (out_mode == OMODE_TERMINAL)
                        fprintf(out_fh, SEQ_RESET);
                    break;
            }
        }
        fprintf(out_fh, "\n");
    }
    if (out_mode == OMODE_HTML)
        fprintf(out_fh, SEQ_HTML_END);

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
