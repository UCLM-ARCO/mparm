OPT       += #-DWITH_CIFF_NODE
DEBUGOPTS += -DWITH_INI_DEBUG
PLATFLAGS += -DSTBUSBUILD

#String to print during build process
MSGSTRING += "ST_NODE   "

ifdef WITH_POWER_NODE
            
  INCDIR += -I${STNODEPOWER_BASEDIR} \
            -I${STNODEPOWER_BASEDIR}/ast_power_full_linux_bin-R.2003.12.1/include/power_stbus_sysc_lib \
            -I${STNODEPOWER_BASEDIR}/glib-2.6.4 \
            -I${STNODEPOWER_BASEDIR}/glib-2.6.4/glib \
            -I${STNODEPOWER_BASEDIR}/glib-2.6.4/gmodule \
            -I${STNODEPOWER_BASEDIR}/glib-2.6.4/gthread \
            -I${STNODEPOWER_BASEDIR}/gsl-1.3/include/gsl 

  LIBDIR += -L${STNODEPOWER_BASEDIR}/ast_power_full_linux_bin-R.2003.12.1/lib \
            -L${STNODEPOWER_BASEDIR}/glib-2.6.4/glib \
            -L${STNODEPOWER_BASEDIR}/glib-2.6.4/glib \
            -L${STNODEPOWER_BASEDIR}/glib-2.6.4/gmodule \
            -L${STNODEPOWER_BASEDIR}/glib-2.6.4/gthread \
            -L${STNODEPOWER_BASEDIR}/gsl-1.3/lib 
  
  LIBS   += -lsyscpow -L${STNODEPOWER_BASEDIR}/ast_power_full_linux_bin-R.2003.12.1/lib \
            -lxp_cpp -lxp -lxpconfigpars -lglib -lgsl -lgslcblas -lm 
                
  CFLAGS += -DWITH_POWER_NODE \
            -I${STNODEPOWER_BASEDIR}/ast_power_full_linux_bin-R.2003.12.1/include/xp \
            -I${STNODEPOWER_BASEDIR}/ast_power_full_linux_bin-R.2003.12.1/include
endif


# Currently the snoop feature is experimental and disabled for default build.
# To enable it, add SNOOP=1 to the make command line.
ifndef SNOOP
  CFLAGS += -DNOSNOOP
endif

INCDIR    += -I$(SYSTEMC_ST_NODE)/COMMON/inc -I$(SYSTEMC_ST_NODE)/COMMON/src \
             -I$(SYSTEMC_ST_NODE)/INITIATOR/src -I$(SYSTEMC_ST_NODE)/TARGET/src -I$(SYSTEMC_ST_NODE)/NODE/src \
             -I$(SYSTEMC_ST_NODE)/genconv -I$(SWARMDIR)/st
             
CPPSRCS   += st_builder.cpp st_signal.cpp st_trace.cpp \
             st_dma.cpp snoopdev.cpp
             
LIBS      += -lstdc++

VPATH     += $(SWARMDIR)/st

st: printmessage $(EXE)


