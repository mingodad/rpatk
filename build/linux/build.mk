all:
	+make -C $(SRCDIR)/rlib/build/$(OS)/$(ARCHDIR) all
	+make -C $(SRCDIR)/rast/build/$(OS)/$(ARCHDIR) all
	+make -C $(SRCDIR)/rpa/build/$(OS)/$(ARCHDIR) all
	+make -C $(SRCDIR)/rvm/build/$(OS)/$(ARCHDIR) all
	+make -C $(SRCDIR)/tests/build/$(OS)/$(ARCHDIR) all
	+make -C $(SRCDIR)/rgrep/unix all

distclean: clean
	+make -C $(SRCDIR)/rlib/build/$(OS)/$(ARCHDIR) distclean
	+make -C $(SRCDIR)/rast/build/$(OS)/$(ARCHDIR) distclean
	+make -C $(SRCDIR)/rpa/build/$(OS)/$(ARCHDIR) distclean
	+make -C $(SRCDIR)/rvm/build/$(OS)/$(ARCHDIR) distclean
	+make -C $(SRCDIR)/tests/build/$(OS)/$(ARCHDIR) distclean
	+make -C $(SRCDIR)/rgrep/unix distclean

clean:
	+make -C $(SRCDIR)/rlib/build/$(OS)/$(ARCHDIR) clean
	+make -C $(SRCDIR)/rast/build/$(OS)/$(ARCHDIR) clean
	+make -C $(SRCDIR)/rpa/build/$(OS)/$(ARCHDIR) clean
	+make -C $(SRCDIR)/rvm/build/$(OS)/$(ARCHDIR) clean
	+make -C $(SRCDIR)/tests/build/$(OS)/$(ARCHDIR) clean
	+make -C $(SRCDIR)/rgrep/unix clean
