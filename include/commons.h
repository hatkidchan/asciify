#ifndef _COMMONS_H
#define _COMMONS_H
#define SEQ_TRUECOLOR_FG "\033[38;2;%d;%d;%dm"
#define SEQ_TRUECOLOR_BG "\033[48;2;%d;%d;%dm"
#define SEQ_TRUECOLOR_BOTH "\033[48;2;%d;%d;%d;38;2;%d;%d;%dm"
#define SEQ_VT100_FG "\033[38;5;%dm"
#define SEQ_VT100_BG "\033[48;5;%dm"
#define SEQ_VT100_BOTH "\033[48;5;%d;38;5;%dm"
#define SEQ_RESET "\033[0m"
#define SEQ_HTML_FG "<span style='color:rgb(%d,%d,%d);display:inline-block;'>"
#define SEQ_HTML_BG "<span style='background:rgb(%d,%d,%d);display:inline-block;'>"
#define SEQ_HTML_BOTH "<span style='background:rgb(%d,%d,%d);color:rgb(%d,%d,%d);display:inline-block;>"
#define SEQ_HTML_CLOSE "</span>"
#define SEQ_HTML_BEGIN "<pre style='background:#111;color:#fff;'>"
#define SEQ_HTML_END "</pre>"
#define SEQ_HTML_NL "<br>"
#define DBGF(f, args...) if (verbose) fprintf(stderr, f, args)
#define DBG(f)           if (verbose) fprintf(stderr, f)

int clamp(int v, int a, int b);

#endif
