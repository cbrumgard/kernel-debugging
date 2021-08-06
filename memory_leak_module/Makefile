obj-m += memory_leaker.o

EXTRA_CFLAGS=-O0 -g -Wno-missing-attributes
export EXTRA_CFLAGS

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
