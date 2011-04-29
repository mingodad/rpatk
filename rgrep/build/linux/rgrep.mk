RLIB_SRCDIR = $(SRCDIR)/rlib
RVM_SRCDIR = $(SRCDIR)/rvm
RPA_SRCDIR = $(SRCDIR)/rpa2
RGREP_SRCDIR = $(SRCDIR)/rgrep
INCLUDE = -I$(SRCDIR)/arch/$(OS)/$(ARCHDIR) -I$(RLIB_SRCDIR) -I$(RVM_SRCDIR) -I$(RPA_SRCDIR) -I$(RGREP_SRCDIR) -I$(RGREP_SRCDIR)/unix


LIBS = -L$(RLIB_SRCDIR)/build/$(OS)/$(ARCHDIR)/out 
LIBS += -L$(RVM_SRCDIR)/build/$(OS)/$(ARCHDIR)/out 
LIBS += -L$(RPA_SRCDIR)/build/$(OS)/$(ARCHDIR)/out 
LIBS += -lrpa2 -lrvm -lrlib -lpthread -lm --static
OBJECTS	= $(OUTDIR)/rpagrep.o $(OUTDIR)/main.o $(OUTDIR)/fsenum.o $(OUTDIR)/rpagrepdep.o $(OUTDIR)/rpagreputf.o
RPAGREP	= rgrep


all : $(OUTDIR) $(OUTDIR)/$(RPAGREP)

$(OUTDIR)/$(RPAGREP) : $(OBJECTS)
	$(CC) $(ARCH) -o $@ $^ $(LIBS)


$(OUTDIR)/%.o: $(RGREP_SRCDIR)/%.c
	+ $(CC) $(CFLAGS) -o $(OUTDIR)/$*.o -c $(RGREP_SRCDIR)/$*.c $(INCLUDE)

$(OUTDIR)/%.o: $(RGREP_SRCDIR)/unix/%.c
	+ $(CC) $(CFLAGS) -o $(OUTDIR)/$*.o -c $(RGREP_SRCDIR)/unix/$*.c $(INCLUDE)


$(OUTDIR):
	@mkdir $(OUTDIR)

distclean: clean
	@rm -f .depend
	@rm -rf $(OUTDIR)

clean:
	@rm -f $(TESTS)
	@rm -f *~
	@rm -f $(SRCDIR)/*~
	@rm -f $(OBJECTS)
	@rm -f $(OUTDIR)/$(RPAGREP)


