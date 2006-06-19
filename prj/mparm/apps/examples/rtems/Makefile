SUBDIRS= hello hello++ basics pthread icee

all:
	for i in $(SUBDIRS) ; do $(MAKE) -C $$i ; done

clean:
	$(RM) *~
	for i in $(SUBDIRS) ; do $(MAKE) -C $$i clean ; done
