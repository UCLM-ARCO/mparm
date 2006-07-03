SUBDIRS=rtems-build uclinux-build

all:
	for i in $(SUBDIRS) ; do $(MAKE) -C $$i all ; done

dat:
	for i in $(SUBDIRS) ; do $(MAKE) -C $$i dat ; done

clean:
	for i in $(SUBDIRS) ; do $(MAKE) -C $$i clean ; done
	$(RM) *~
