SUBDIRS= $(shell find . -maxdepth 1 -type d -a ! -name .\* )

all:
	for i in $(SUBDIRS) ; do $(MAKE) -C $$i ; done

clean:
	$(RM) *~
	for i in $(SUBDIRS) ; do $(MAKE) -C $$i clean ; done
