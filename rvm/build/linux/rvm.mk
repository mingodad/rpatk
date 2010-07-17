RVM_SRCDIR = $(SRCDIR)/rvm
RVM_LIB = $(OUTDIR)/librvm.a
RVM_SO = $(OUTDIR)/librvm.so.1.0

RVM_OBJECTS =	\
	$(OUTDIR)/rvm.o \


ifeq ($(OS), linux)
all: $(OUTDIR) $(RVM_LIB) $(RVM_SO)
else
all: $(OUTDIR) $(RVM_LIB)
endif


$(OUTDIR)/%.o: $(RVM_SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $(OUTDIR)/$*.o -c $(RVM_SRCDIR)/$*.c

$(RVM_LIB): $(RVM_OBJECTS)
	$(AR) -cr $@ $^

$(RVM_SO): $(RVM_OBJECTS)
	$(CC) $(LDFLAGS) -shared -Wl,-soname,librvm.so -o $@ $^

$(OUTDIR):
	@mkdir $(OUTDIR)

distclean: clean
	@rm -f .depend
	@rm -rf $(OUTDIR)

clean:
	@rm -f $(RVM_LIB)
	@rm -f $(RVM_SO)
	@rm -f $(RVM_OBJECTS)
	@rm -f *~
	@rm -f $(SRCDIR)/*~

