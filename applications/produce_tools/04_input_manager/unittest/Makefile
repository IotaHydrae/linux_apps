#
# Makefile
#
CC ?= gcc
WARNINGS ?= -Wall
CFLAGS ?= -O3 -g0  $(WARNINGS)

THIS_CSRCS := $(shell find -L ./ -name *.c)

CSUFFIX = $(CSRCS:.c=)

all:deafult

deafult:
	$(foreach n, $(CSRCS),$(CC) $(CFLAGS) $(n) -o $(n:.c=) $(LDFLAGS);)

clean: 
	rm -f  $(CSUFFIX)
