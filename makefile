# Makefile for cand

# Setup variables
IDIR   = .
ODIR   = obj
BDIR   = bin
_LIBS  = 
_DEPS  = 
_OBJS  = main.o
_BIN   = cand
_CFLAGS= -D_XOPEN_SOURCE=500 -std=c89 -pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wno-missing-field-initializers -Wformat=2 -Wunused -Wunused-result
CC     = gcc

# Proccess variables accordingly
LIBS   = $(patsubst %,-l%,$(_LIBS))
DEPS   = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJS   = $(patsubst %,$(ODIR)/%,$(_OBJS))
BIN    = $(BDIR)/$(_BIN)
CFLAGS =-I$(IDIR) $(_CFLAGS)

# Setup default target, all
.DEFAULT_GOAL := all
.PHONY: default
default: all
all: $(BIN) ./makefile

# Setup clean target
.PHONY: clean
clean:
	@rm -f $(ODIR)/*.o
	@rm -f $(BIN)

# Link neccessary sources into objects
$(ODIR)/%.o: %.c $(DEPS) | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

# Create object folder if missing
$(ODIR):
	@mkdir -p $@

# Compile objects into binary
$(BIN): $(OBJS) | $(BDIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# Create bin folder if missing
$(BDIR):
	@mkdir -p $@
