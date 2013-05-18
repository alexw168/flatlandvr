#
# Makefile
#
# K. L. Summers
# Nov 1, 2002
#

# Language to compile with ("C" or "CPP")
LANG = "CPP"

# These are the source files (*.c, not *.h)
SOURCE = intersect.c

# These are the externally required headers (*.h that others need to include)
EXTHEADERS = BasicIntersect.h

# Name of the library to be produced
LIBNAME =  libBasicIntersection.so

#-- There should be no need to modify below this line --#

# include the ./configure determined variables
include ../../make-include

ifeq ($(OPSYS), IRIX64)
  COPTS += -DUSE_OLD_CPP_HEADERS
endif

ifeq ($(OPSYS), IRIX)
  COPTS += -DUSE_OLD_CPP_HEADERS
endif

# dependancy compilation rule (makes a .d for every .c, which we include later)
%.d: %.c
	$(SHELL) -ec '$(COMPILER) -M $(COPTS) $(CFLAGS) \
	$(INCLUDE) $< | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
	[ -s $@ ] || rm -f $@'

# Main compilation rule
%.o:%.c
	$(COMPILER) $(COPTS) $(CFLAGS) $(INCLUDE) $<


all:	$(FLLIBDIR)/$(LIBNAME)

# include the mini-makefiles (*.d) for dependancy rules
ifneq ($(MAKECMDGOALS),clean)
 ifneq ($(MAKECMDGOALS),veryclean)
  ifneq ($(MAKECMDGOALS),opsys)
   include $(OBJECT:.o=.d)
  endif
 endif
endif

# link rule
$(FLLIBDIR)/$(LIBNAME): $(LINK_DEPS) 
	$(COMPILER) $(LINKOPTS) -shared -o $(LIBNAME) $(OBJECT)
	cp $(EXTHEADERS) $(FLINCDIR)
	cp $(LIBNAME) $(FLLIBDIR)

opsys:
	echo $(OPSYS)
	echo $(COMPILER) $(COPTS)

clean:
	-$(RM) $(OBJECT) $(LIBNAME) so_locations

veryclean: clean
	-$(RM) $(OBJECT:.o=.d)
