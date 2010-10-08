# required
OPENWIZSDK	= /home/sharek/arm-openwiz-linux-gnu

# wiz specific
TARGET_GCC	= $(OPENWIZSDK)/bin/arm-openwiz-linux-gnu-gcc
TARGET_AR	= $(OPENWIZSDK)/bin/arm-openwiz-linux-gnu-ar
TARGET_CFLAGS	= -Wall -DWIZ -I/home/sharek/arm-openwiz-linux-gnu/include
TARGET_LIBS	= -L$(OPENWIZSDK)/arm-openwiz-linux-gnu/lib -L$(OPENWIZSDK)/lib 

# host machine version
HOST_GCC	= gcc
HOST_AR		= ar
HOST_CFLAGS	= -Wall -g -D_DEBUG
HOST_LIBS	= 

all:	target host sample1
sample1:
	dd if=/dev/urandom of=testfile.bin bs=1M count=1
	dd if=/dev/urandom of=testfile2.bin bs=1M count=1
	dd if=/dev/urandom of=testfile3.bin bs=1M count=4

	./wizPacker.bin wizUnpackerSample1.bin testfile2.bin testfile3.bin testfile.bin
	./wizPacker.bin wizUnpackerSample1.gpe testfile.bin
	md5sum testfile.bin
	rm testfile.bin
	./wizUnpackerSample1.bin
	md5sum testfile.bin
target:
	$(TARGET_GCC) $(TARGET_CFLAGS) $(TARGET_LIBS) wizUnpackerSample1.c -o wizUnpackerSample1.gpe
host:
	$(HOST_GCC)   $(HOST_CFLAGS)   $(HOST_LIBS)   wizPacker.c -o wizPacker.bin
	$(HOST_GCC)   $(HOST_CFLAGS)   $(HOST_LIBS)   wizUnpackerSample1.c -o wizUnpackerSample1.bin

