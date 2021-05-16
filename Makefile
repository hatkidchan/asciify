# You will need to use GNU make

INCLUDES := $(realpath $(CURDIR)/include)
CFLAGS += -pedantic -Wall -Wextra -Wno-unused-but-set-variable \
	-Wno-sign-compare -Wno-variadic-macros


LIBOBJ := build/color.o build/commons.o \
	build/image_aspect.o \
	build/stb_image.o build/stb_image_resize.o \
	build/stb_image_write.o build/stb_truetype.o

SRCOBJ := build/asciify_braille build/asciify_blocks build/asciify_charmap

all: lib src

clean:
	rm $(LIBOBJ) $(SRCOBJ)

lib: $(LIBOBJ) 

src: $(SRCOBJ)

build/%.o: lib/%.c
	$(CC) $(CFLAGS) $^ -I$(INCLUDES) -c -o $@ 

build/asciify_blocks: lib
	$(CC) $(LIBOBJ) src/asciify_blocks.c \
	-lm -I$(INCLUDES) $(CFLAGS) -o $@\

build/asciify_charmap: lib
	$(CC) $(LIBOBJ) src/asciify_charmap.c \
	-lm -I$(INCLUDES) $(CFLAGS) -o $@ \

build/asciify_braille: lib
	$(CC) $(LIBOBJ) src/asciify_braille.c \
	-lm -I$(INCLUDES) $(CFLAGS) -o $@ \

.PHONY: all lib src
