
CC = arm-linux-gcc
OBJCOPY = arm-linux-objcopy

INCLUDE = -I./header
#CFLAGS = -O2 -Wall -march=armv4 -mtune=strongarm -fomit-frame-pointer -mapcs-32 -nostdinc -ffreestanding $(INCLUDE)
CFLAGS = -O2 -Wall -march=armv4 -mtune=strongarm -fomit-frame-pointer -mapcs-32 -nostdinc $(INCLUDE)
DEBUG_FLAGS = -D_DEBUG_MSG

OCFLAGS = -O binary -R .note -R .comment -S
LDFLAGS = -static -nostdlib -Wl,-T,./header/ld-xscale 

COMPILE = $(CC) $(CFLAGS) $(DEBUG_FLAGS)

SOURCE = start.S serial.c time.c lib.c smc91111.c net.c bootp.c tftp.c flash.c commands.c main.c
OBJECT = start.o serial.o time.o lib.o smc91111.o net.o bootp.o tftp.o flash.o commands.o main.o

TARGET_IMAGE = empos_boot

all : $(OBJECT)
	$(CC) $(INCLUDE) $(CFLAGS) $(LDFLAGS) -o $(TARGET_IMAGE)-elf32 $(OBJECT) -lgcc
	$(OBJCOPY) $(OCFLAGS) $(TARGET_IMAGE)-elf32 $(TARGET_IMAGE)
#	sudo cp $(TARGET_IMAGE) /tftpboot
.S.o :
	$(COMPILE) -c $<

.c.o :
	$(COMPILE) -c $<

clean :
	rm -rf *.o
	rm -rf $(TARGET_IMAGE) $(TARGET_IMAGE)-elf32
