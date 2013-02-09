DIRS = acpidump acpixtract madt turbostat

all:
	for i in $(DIRS); do $(MAKE) -C $$i $@; done

clean:
	for i in $(DIRS); do $(MAKE) -C $$i $@; done

.PHONY: all clean
