include ../Config

BINDIR = ../bin
TARGETS = $(BINDIR)/array_custom\
		  $(BINDIR)/main\
INCS = -I../include
LIBS = -L../lib -lzfp
CLIBS = $(LIBS) $(LDFLAGS) -lm
CXXLIBS = $(LIBS) $(LDFLAGS)

all: $(TARGETS)

$(BINDIR)/simple: array_custom.c ../lib/$(LIBZFP)
	$(CC) $(CFLAGS) $(INCS) array_custom.c $(CLIBS) -o $@

$(BINDIR)/simple: main.c ../lib/$(LIBZFP)
	$(CC) $(CFLAGS) $(INCS) mina.c $(CLIBS) -o $@
