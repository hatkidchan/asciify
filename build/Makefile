# Sorry but you need to use GNU Make
PROJDIR := $(realpath $(CURDIR)/.. )
LIBSDIR := $(PROJDIR)/lib
SOURCEDIR := $(PROJDIR)/src
INCLUDES := $(PROJDIR)/include

LIBSOURCES = $(foreach file, $(LIBSDIR),$(wildcard $(file)/*.c))

all: asciify_base asciify_blocks asciify_charmap lib

lib:
	$(CC) -lm $(LIBSOURCES) -I$(INCLUDES) $(CFLAGS) -c 
LIBOBJ := $(foreach file, $(CURDIR),$(wildcard $(file)/*.o))

asciify_base: $(LIBOBJ) lib
	$(CC) -lm  -I$(INCLUDES) $(CFLAGS) -o $@ \
	$(LIBOBJ) $(SOURCEDIR)/asciify_base.c

asciify_blocks: $(LIBOBJ) lib
	$(CC) -lm  -I$(INCLUDES) $(CFLAGS) -o $@ \
	$(LIBOBJ) $(SOURCEDIR)/asciify_blocks.c

asciify_charmap: $(LIBOBJ) lib
	$(CC) -lm -I$(INCLUDES) $(CFLAGS) -o $@ \
	$(LIBOBJ) $(SOURCEDIR)/asciify_charmap.c

clean: 
	rm *.o asciify_base asciify_blocks asciify_charmap

.PHONY: lib all