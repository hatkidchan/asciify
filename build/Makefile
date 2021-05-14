PROJDIR := $(realpath $(CURDIR)/.. )
LIBSDIR := $(PROJDIR)/lib
SOURCEDIR := $(PROJDIR)/src
INCLUDES := $(PROJDIR)/include

LIBSOURCES = $(foreach file, $(LIBSDIR),$(wildcard $(file)/*.c))
SRCSOURCES = $(foreach file, $(SOURCEDIR),$(wildcard $(file)/*.c))

all:lib src

lib:
	$(CC) -v $(LIBSOURCES) -I$(INCLUDES) $(CFLAGS) -c 

src:
	$(CC) -v $(SRCSOURCES) -I$(INCLUDES) $(CFLAGS) 

.PHONY: lib src all