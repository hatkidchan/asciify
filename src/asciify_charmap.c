#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include "stb_image.h"
#include "stb_image_resize.h"
#include "color.h"
#include "commons.h"
#include "image_aspect.h"

enum { MODE_BW = 0, MODE_256 = 1, MODE_TRUECOLOR = 2, MODE_DANSI = 3 };
enum { OMODE_TERMINAL = 0, OMODE_HTML = 1 };

const struct color_t dansi_palette[8] = {
  { 0x4f, 0x54, 0x5c, 0 },
  { 0xd1, 0x31, 0x35, 0 },
  { 0x85, 0x99, 0x00, 0 },
  { 0xb5, 0x89, 0x00, 0 },
  { 0x26, 0x8b, 0xd2, 0 },
  { 0xd3, 0x36, 0x82, 0 },
  { 0xd3, 0x36, 0x82, 0 },
  { 0xff, 0xff, 0xff, 0 },
};

uint8_t dansi_pick(color_t color);

void usage(char *progname)
{
    fprintf(stderr, "usage: %s [-hvT] [-o FILE] [-W WIDTH] [-H HEIGHT] [-S CHARS] FILE\n",
            progname);
    fprintf(stderr, "\n");
    fprintf(stderr, "-h\t\tShow this help\n");
    fprintf(stderr, "-v\t\tBe verbose\n");
    fprintf(stderr, "-G\t\tSet mode to black-and-white\n");
    fprintf(stderr, "-D\t\tSet mode to Discord ANSI syntax\n");
    fprintf(stderr, "-T\t\tUse TrueColor\n");
    fprintf(stderr, "-M\t\tOutput as HTML\n");
    fprintf(stderr, "-S CHARS\tList of characters to use\n");
    fprintf(stderr, "\t\tWill be a good idea to avoid multibyte characters.\n");
    fprintf(stderr, "-o FILE\t\tOutput filename (- or stdout by default)\n");
    fprintf(stderr, "-W WIDTH\tResult width (in characters)\n");
    fprintf(stderr, "-H HEIGHT\tResult height (in characters)\n");
    fprintf(stderr, "FILE\t\tInput image\n");
}

int main(int argc, char **argv)
{
    int in_w, in_h, out_w = 80, out_h = 24, res_w = out_w, res_h = out_h, n_ch;
    char *in_filename = NULL,
         *out_filename = NULL,
         *charset = " .`-_*=@#";

    bool verbose = false;
    int mode = MODE_256, out_mode = OMODE_TERMINAL;
    
    FILE *out_fh = stdout;

    int c;
    while ((c = getopt(argc, argv, "vDMGTho:W:H:S:")) != -1)
    {
        switch (c)
        {
            case 'G':
                mode = MODE_BW;
                break;
            case 'T':
                mode = MODE_TRUECOLOR;
                break;
            case 'D':
                mode = MODE_DANSI;
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
            case 'S':
                charset = optarg;
                if (strlen(charset) < 1)
                {
                    fprintf(stderr, "Error: charset is too small\n");
                    return 2;
                }
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
                if (optopt == 'o' || optopt == 'W' 
                    || optopt == 'H' || optopt == 'S')
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

    color_t *source_im, *scaled_im, pix;
    source_im = (color_t *) stbi_load(in_filename, 
            &in_w, &in_h, &n_ch, STBI_rgb_alpha);
    
    DBGF("Source image size: %d:%d\n", in_w, in_h);
    DBG("Resizing image to ");
    get_size_keep_aspect(in_w * 2, in_h, out_w, out_h, &res_w, &res_h);
    DBGF("%d:%d\n", res_w , res_h);
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

    if (out_mode == OMODE_HTML)
        fprintf(out_fh, SEQ_HTML_BEGIN);
    
    if (mode == MODE_DANSI)
        fprintf(out_fh, "```ansi\n");

    int charlen = strlen(charset);
    for (int y = 0; y < res_h - 1; y++)
    {
        color_t last_color = { 0, 0, 0, 0 };
        for (int x = 0; x < res_w; x++)
        {
            pix = scaled_im[x + res_w * y];
            pix.a = 255;
            float brightness = color_grayscale(pix);
            char sym = charset[(int)(brightness * (charlen - 1))];

            switch (mode)
            {
                case MODE_256:
                    if ((color_to_vt100(last_color) != color_to_vt100(pix))
                        || last_color.a == 0)
                    {
                        if (out_mode == OMODE_HTML)
                        {
                            if (x > 0)
                                fprintf(out_fh, SEQ_HTML_CLOSE);
                            pix = color_clamp_vt100(pix);
                            fprintf(out_fh, SEQ_HTML_FG, pix.r, pix.g, pix.b);
                        }
                        else
                        {
                            fprintf(out_fh, SEQ_VT100_FG, color_to_vt100(pix));
                        }
                    }
                    break;
                case MODE_TRUECOLOR:
                    if (last_color.r != pix.r
                        || last_color.g != pix.g
                        || last_color.b != pix.b
                        || last_color.a == 0)
                    {
                        if (out_mode == OMODE_HTML)
                        {
                            if (x > 0)
                                fprintf(out_fh, SEQ_HTML_CLOSE);
                            fprintf(out_fh, SEQ_HTML_FG, pix.r, pix.g, pix.b);
                        }
                        else
                        {
                            fprintf(out_fh, SEQ_TRUECOLOR_FG,
                                    pix.r, pix.g, pix.b);
                        }
                    }
                    break;
                case MODE_DANSI:
                    fprintf(out_fh, "\033[%dm", dansi_pick(pix));
                    break;
                default:
                    break;
            }
            last_color = pix;
            fprintf(out_fh, "%c", sym);
        }
        if (mode == MODE_256 || mode == MODE_TRUECOLOR)
            fprintf(out_fh,
                    out_mode == OMODE_HTML ? SEQ_HTML_CLOSE : SEQ_RESET);
        fprintf(out_fh, "\n");
    }
    if (out_mode == OMODE_HTML)
        fprintf(out_fh, SEQ_HTML_END);
    if (mode == MODE_DANSI)
        fprintf(out_fh, "```");

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

uint8_t dansi_pick(color_t color)
{
  uint8_t nearest = 0; int32_t min_distance = 0x0fffffff;
  for (uint8_t i = 0; i < 8; i++)
  {
    int16_t dr = dansi_palette[i].r - color.r;
    int16_t dg = dansi_palette[i].g - color.g;
    int16_t db = dansi_palette[i].b - color.b;
    int32_t distance = dr * dr + dg * dg + db * db;
    if (distance < min_distance)
    {
      min_distance = distance;
      nearest = i;
    }
  }
  return 30 + nearest;
}
