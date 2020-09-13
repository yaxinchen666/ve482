obj-m := dadfs.o
dadfs-objs := base.o
ccflags-y := -DDADFS_DEBUG
#LIB_PATH = /usr/src/linux-source-4.15.0/linux-source-4.15.0

all: ko mkfs-dadfs

ko:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

mkfs-dadfs_SOURCES:
	mkfs-dadfs.c dadfs.h

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm mkfs-dadfs
