obj-m += find_fb.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=`pwd` modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=`pwd` clean
