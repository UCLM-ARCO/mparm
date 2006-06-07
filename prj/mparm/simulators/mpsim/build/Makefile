include Makefile.defs

ifndef SWARMDIR
  $(error Fatal error: Undefined SWARMDIR environment variable!)
endif




############## Initialization of core includes, libs, sources
INCDIR   += -I. -I.. -I$(SYSTEMC)/include -I$(SWARMDIR)/core -I$(SWARMDIR)/swarm
LIBDIR   += -L$(SYSTEMC)/lib-$(SYSC_TARGET_ARCH)
LIBS     += -lsystemc20
CPPSRCS  += address.cpp clock_tree.cpp core_signal.cpp debug.cpp dmacontrol.cpp dmatransfer.cpp \
            dual_clock_fifo.cpp dual_clock_pinout_adapter.cpp dual_clock_pinout_master_if.cpp \
            dual_clock_pinout_slave_if.cpp freq_register.cpp globals.cpp main.cpp mem_class.cpp \
            ocp_memory.cpp ocp_slave.cpp ocp_tester.cpp parser.cpp power.cpp read_elf.cpp \
            reset.cpp smartmem_signal.cpp sim_support.cpp stats.cpp transaction_gen.cpp
CSRCS    +=
CPPOBJS   = $(CPPSRCS:.cpp=.o)
COBJS     = $(CSRCS:.c=.o)
OBJS      = $(CPPOBJS) $(COBJS)
VPATH    += $(SWARMDIR)/core
EXE       = $(SWARMDIR)/$(MODULE).x




############## Inclusion of relevant architectural modules
ifeq ($(findstring amba-ahb,$(MAKECMDGOALS)),amba-ahb)
  include Makefile.amba-ahb
endif

ifeq ($(findstring amba-occn,$(MAKECMDGOALS)),amba-occn)
  include Makefile.amba-occn
endif

ifeq ($(findstring amba-sig,$(MAKECMDGOALS)),amba-sig)
  include Makefile.amba-sig
endif

ifeq ($(findstring amba-syn-ahb,$(MAKECMDGOALS)),amba-syn-ahb)
  include Makefile.amba-syn-ahb
endif

ifeq ($(findstring amba-syn-axi,$(MAKECMDGOALS)),amba-syn-axi)
  include Makefile.amba-syn-axi
endif

ifeq ($(findstring lx,$(MAKECMDGOALS)),lx)
  include Makefile.lx
endif

ifeq ($(findstring powerpc,$(MAKECMDGOALS)),powerpc)
  include Makefile.powerpc
endif

ifeq ($(findstring simitarm,$(MAKECMDGOALS)),simitarm)
  include Makefile.simitarm
endif

ifeq ($(findstring st,$(MAKECMDGOALS)),st)
  include Makefile.st
endif

ifeq ($(findstring swarm,$(MAKECMDGOALS)),swarm)
  include Makefile.swarm
endif

ifeq ($(findstring tg,$(MAKECMDGOALS)),tg)
  include Makefile.tg
endif

ifeq ($(findstring xpipes,$(MAKECMDGOALS)),xpipes)
  include Makefile.xpipes
endif

ifeq ($(findstring dram,$(MAKECMDGOALS)),dram)
  include Makefile.dram
endif

ifeq ($(IP_TG), 1) 
  include Makefile.ip_tg
endif

ifeq ($(findstring all,$(MAKECMDGOALS)),all)
  include Makefile.amba-ahb
  include Makefile.amba-occn
  include Makefile.amba-sig
  include Makefile.amba-syn-ahb
  include Makefile.amba-syn-axi
  include Makefile.lx
  include Makefile.simitarm
  include Makefile.st
  include Makefile.swarm
  include Makefile.powerpc
  include Makefile.tg
  include Makefile.xpipes
  include Makefile.dram
  include Makefile.ip_tg
endif




############## Choice of compiler options to use 
ifeq ($(OPTIONSET),QUIET)
  CFLAGS    += ${OPT} ${INCDIR} ${LIBDIR} ${DEFAULTFLAGS} ${PLATFLAGS}
  CPPFLAGS  += ${CFLAGS}
endif
ifeq ($(OPTIONSET),VERBOSE)
  CFLAGS    += ${DEBUGOPTS} ${INCDIR} ${LIBDIR} ${DEFAULTFLAGS} ${PLATFLAGS}
  CPPFLAGS  += ${CFLAGS}
endif
ifdef REDUCED
  CFLAGS += -DREDUCED_RELEASE
endif



###### Parse Command Line For Verbose Flag
QUIET = @
ECHO  =
ifeq ($(BUILDOPTIONS),VERBOSE)
    QUIET =
    ECHO  = > /dev/null
endif



############## Actual build process: executable, objects, dependencies
all: amba-ahb amba-occn amba-sig amba-syn-ahb amba-syn-axi lx powerpc simitarm st swarmiss tg xpipes dram


$(EXE): $(OBJS)
	$(QUIET)test -d "$(SWARMDIR)/bin" || mkdir "$(SWARMDIR)/bin" 
	$(QUIET)$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) 2>&1 | c++filt
	@echo $(ECHO)
	@echo "  [$(LINK)LINK$(DEFAULTCOLOR)]  :  $@"$(ECHO)
	@echo $(ECHO)
        
%.o:    %.cpp
	$(QUIET)$(CC) $(CFLAGS) -c $<
	@echo "  [$(COMPILE)CC$(DEFAULTCOLOR)]    :  $@"$(ECHO)

%.dpp:  %.cpp
	@echo "  [$(DEPENDENCY)DEP$(DEFAULTCOLOR)]   :  $@"$(ECHO)
	$(QUIET)set -e; $(CC) $(CFLAGS) -MM $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; [ -s $@ ] || rm -f $@
        
%.d:    %.c
	@echo "  [$(DEPENDENCY)DEP$(DEFAULTCOLOR)]   :  $@"$(ECHO)
	$(QUIET)set -e; $(CC) $(CFLAGS) -MM $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; [ -s $@ ] || rm -f $@




############## Cleanup routines
.PHONY : clean
.PHONY : cleanexe
.PHONY : cleanmain
.PHONY : printmessage
clean:
	@echo
	rm -f $(SWARMDIR)/build/*.o $(SWARMDIR)/build/*.dpp $(SWARMDIR)/build/*.d $(EXE)
	@echo
cleanexe:
	@echo
	rm -f $(EXE)
	@echo
cleanmain:
	@echo
	$(SWARMDIR)/build/main.*
	@echo
printmessage:
	@echo
	@echo Building platform with: $(MSGSTRING)
	@echo
	@echo Compilation line is: $(ECHO)
	@echo $(ECHO)
	@echo $(CC) $(CFLAGS) $(ECHO)
	@echo $(ECHO)



############## Inclusion of dependencies. Beware: set to discard warnings, so it won't notice errors...
ifneq ($(MAKECMDGOALS),cleanmain)
  ifneq ($(MAKECMDGOALS),clean)
    ifneq ($(MAKECMDGOALS),cleanexe)
      ifeq ($(findstring cpp,$(CPPSRCS)),cpp)
        -include $(CPPSRCS:.cpp=.dpp)
      endif
      ifeq ($(findstring c,$(CSRCS)),c)
        -include $(CSRCS:.c=.d)
      endif
    endif
  endif
endif




############## Color Codes
RED          = [01;31m
GREEN        = [01;32m
YELLOW       = [01;33m
BLUE         = [01;34m
VIOLET       = [01;35m
CYAN         = [01;36m
WHITE        = [01;37m
DEFAULTCOLOR = [00m
PLAIN        =
