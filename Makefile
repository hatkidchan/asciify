INCLUDES := $(realpath $(CURDIR)/include)


all: build/color.o build/image_aspect.o \
	build/stb_image.o build/stb_image_resize.o \
	build/stb_image_write.o build/stb_truetype.o

build/color.o: lib/color.c
	$(CC) $^ -I$(INCLUDES) -c

build/image_aspect.o: lib/image_aspect.c
	$(CC)  $^ -I$(INCLUDES) -c

build/stb_image.o: lib/stb_image.c
	$(CC)  $^ -I$(INCLUDES) -c

build/stb_image_resize.o: lib/stb_image_resize.c
	$(CC)  $^ -I$(INCLUDES) -c

build/stb_image_write.o: lib/stb_image_write.c
	$(CC)  $^ -I$(INCLUDES) -c

build/stb_truetype.o: lib/stb_truetype.c
	$(CC)  $^ -I$(INCLUDES) -c
