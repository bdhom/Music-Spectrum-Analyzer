CFLAGS=-g -O2 -Wall -Wextra -Isrc -DNDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTFLAGS)
PREFIX?=/usr/local

CC=gcc
PROGNAME=wave_generator
SOURCEDIR=src
BINDIR=build
DBGBINDIR=$(BINDIR)/debug
RLSBINDIR=$(BINDIR)/release

SOURCES=$(wildcard $(SOURCEDIR)/**/*.c $(SOURCEDIR)/*.c)
OBJECTS=$(patsubst $(SOURCEDIR)%.c,$(RLSBINDIR)%.o,$(SOURCES))
DBGOBJECTS=$(patsubst $(SOURCEDIR)%.c,$(DBGBINDIR)%.o,$(SOURCES))

TARGET=$(RLSBINDIR)/$(PROGNAME)
DBGTARGET=$(DBGBINDIR)/$(PROGNAME)

release: $(TARGET)

debug: CFLAGS=-g -Wall -Wextra -Isrc $(OPTFLAGS)
debug: $(DBGTARGET)

$(DBGBINDIR) $(RLSBINDIR):
	@mkdir -p $@

$(OBJECTS): $(RLSBINDIR)/%.o : $(SOURCEDIR)/%.c | $(RLSBINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DBGOBJECTS): $(DBGBINDIR)/%.o : $(SOURCEDIR)/%.c | $(DBGBINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): CFLAGS+=-fPIC 
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(DBGTARGET): CFLAGS+=-fPIC 
$(DBGTARGET): $(DBGOBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -rf $(BINDIR)
