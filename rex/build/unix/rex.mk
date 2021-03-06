REX_SRCDIR = $(ROOT_DIR)/rex
RLIB_SRCDIR = $(ROOT_DIR)/rlib
RVM_SRCDIR = $(ROOT_DIR)/rvm
RPA_SRCDIR = $(ROOT_DIR)/rpa
REX_LIB = librex.a
REX_SO_VERSION = 2.0
REX_SO_NAME = librex.so
REX_SO = $(REX_SO_NAME).$(REX_SO_VERSION)
TARGET_REX_LIB = $(OUTDIR)/$(REX_LIB)
TARGET_REX_SO = $(OUTDIR)/$(REX_SO)
CFLAGS += -I$(ROOT_DIR)

LIBS = -L$(RLIB_SRCDIR)/build/unix/$(ARCHDIR)/out 
LIBS += -L$(RVM_SRCDIR)/build/unix/$(ARCHDIR)/out 
LIBS += -L$(RPA_SRCDIR)/build/unix/$(ARCHDIR)/out 
LIBS += -L$(REX_SRCDIR)/build/unix/$(ARCHDIR)/out 
LIBS += -lrex -lrpa -lrvm -lrlib -lpthread -lm
LDFLAGS += $(LIBS)


REX_OBJECTS =	\
	$(OUTDIR)/rexdb.o \
	$(OUTDIR)/rexregex.o \
	$(OUTDIR)/rexcompiler.o \
	$(OUTDIR)/rexfragment.o \
	$(OUTDIR)/rexnfasimulator.o \
	$(OUTDIR)/rexdfasimulator.o \
	$(OUTDIR)/rexdfaconv.o \
	$(OUTDIR)/rextransition.o \
	$(OUTDIR)/rexdfa.o \
	$(OUTDIR)/rexstate.o


ifeq ($(OS), linux)
all: $(OUTDIR) $(TARGET_REX_LIB) $(TARGET_REX_SO)
else
all: $(OUTDIR) $(TARGET_REX_LIB)
endif


$(OUTDIR)/%.o: $(REX_SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $(OUTDIR)/$*.o -c $(REX_SRCDIR)/$*.c

$(TARGET_REX_LIB): $(REX_OBJECTS)
	$(AR) -cr $@ $^

$(TARGET_REX_SO): $(REX_OBJECTS)
	$(CC) $(LDFLAGS) -shared -Wl,-soname,$(REX_SO_NAME) -o $@ $^

$(OUTDIR):
	@mkdir $(OUTDIR)

distclean: clean
	-rm -f .depend
	-rm -rf $(OUTDIR)

clean:
	-rm -f $(TARGET_REX_LIB)
	-rm -f $(TARGET_REX_SO)
	-rm -f $(REX_OBJECTS)
	-rm -f *~
	-rm -f $(ROOT_DIR)/*~

install:
	cp $(TARGET_REX_SO) $(RPATK_LIB_INSTALL)
	cp $(TARGET_REX_LIB) $(RPATK_LIB_INSTALL)
	cp $(REX_SRCDIR)/*.h $(RPATK_INC_INSTALL)/rex

uninstall:
	-rm -f $(RPATK_LIB_INSTALL)/$(REX_LIB)
	-rm -f $(RPATK_LIB_INSTALL)/$(REX_SO_NAME)*
	-rm -f $(RPATK_INC_INSTALL)/rex/*
