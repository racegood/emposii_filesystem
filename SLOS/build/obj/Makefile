CC = arm-elf-gcc
ASSEMBLER = arm-elf-as
LINKER = arm-elf-ld
OC = arm-elf-objcopy

CONVERT = arm-elf-strip -O binary -o
CFLAGS = -mapcs -fomit-frame-pointer -O1 -c
ASFLAGS =
LDFLAGS = -p -T ./slos-linkscript
OCFLAGS = -O binary -R .note -R .comment -S	 # objcopy flag

OBJS = start.o \
init.o \
pcb.o \
swiapi.o \
undef.o \
swi.o \
prefetch.o \
data.o \
reserved.o \
irq.o \
fiq.o \
scheduler.o \
swi_handler.o \
irq_handler.o \
bringup.o \
cpsr.o \
event_init.o \
button_service.o \
tick_service.o \
ddf_io.o \
led_driver.o \
serial_driver.o \
segment_driver.o \
blkdev_driver.o \
mutex.o \
cinit.o \
cli_com.o \
cli_readline.o \
cli_shell.o \
cli_serial.o \
uid.o \
task1_start.o \
task2_start.o \
task3_start.o \
debug.o \
serial.o \
lib.o \
system_kernel.o \
file_structure.o \
cmd_program.o \
file_system.o \
iNode.o \
block_init.o

ELF_EXEC = slos-elf32

BIN_IMAGE = slos.bin

all : $(ELF_EXEC)
	$(OC) $(OCFLAGS) $(ELF_EXEC) $(BIN_IMAGE)
	cp slos.bin  /tftpboot/
	mv slos-elf32 ../image/
	mv slos.bin ../image/

$(ELF_EXEC) : $(OBJS)
	$(LINKER) $(LDFLAGS) -o $@ $(OBJS) -Map slos.map

debug.o : ../src/debug.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<

serial.o : ../src/serial/serial.c
	${CC} ${CFLAGS} -o $@ $<

lib.o : ../src/serial/lib.c
	${CC} ${CFLAGS} -o $@ $<

file_system.o : ../src/fs/file_system.c
	${CC} ${CFLAGS} -o $@ $<

system_kernel.o : ../src/fs/system_kernel.c
	${CC} ${CFLAGS} -o $@ $<

cmd_program.o : ../src/fs/cmd_program.c
	${CC} ${CFLAGS} -o $@ $<

file_structure.o : ../src/fs/FileStructure.c
	${CC} ${CFLAGS} -o $@ $<

iNode.o : ../src/fs/iNode.c
	${CC} ${CFLAGS} -o $@ $<

start.o : ../src/start.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
init.o : ../src/e7t/init.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
pcb.o : ../src/e7t/pcb.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
undef.o : ../src/undef.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
swi.o : ../src/swi.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
prefetch.o : ../src/prefetch.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
data.o : ../src/data.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
reserved.o : ../src/reserved.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
irq.o : ../src/irq.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
fiq.o : ../src/fiq.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
scheduler.o : ../src/scheduler.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
irq_handler.o : ../src/e7t/events/irq_handler.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
bringup.o : ../src/events/bringup.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
cpsr.o : ../src/events/cpsr.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
 
event_init.o : ../src/e7t/events/event_init.c
	${CC} ${CFLAGS} -o $@ $<

button_service.o : ../src/e7t/events/button_service.c
	${CC} ${CFLAGS} -o $@ $<
 
swiapi.o : ../src/core/swiapi.c
	${CC} ${CFLAGS} -o $@ $<

swi_handler.o : ../src/e7t/events/swi_handler.c
	${CC} ${CFLAGS} -O1 -c -o $@ $<

tick_service.o : ../src/e7t/events/tick_service.c
	${CC} ${CFLAGS} -o $@ $<
 
ddf_io.o : ../src/devices/ddf_io.c
	${CC} ${CFLAGS} -o $@ $<
 
led_driver.o : ../src/e7t/devices/led_driver.c
	${CC} ${CFLAGS} -o $@ $<
 
serial_driver.o : ../src/e7t/devices/serial_driver.c
	${CC} ${CFLAGS} -o $@ $<
 
segment_driver.o : ../src/e7t/devices/segment_driver.c
	${CC} ${CFLAGS} -o $@ $<
 
blkdev_driver.o : ../src/e7t/devices/blkdev_driver.c
	${CC} ${CFLAGS} -o $@ $<

mutex.o : ../src/core/mutex.c
	${CC} ${CFLAGS} -o $@ $<
 
cinit.o : ../src/core/cinit.c
	${CC} ${CFLAGS} -o $@ $<
 
cli_com.o : ../src/core/cli_com.c
	${CC} ${CFLAGS} -o $@ $<
 
cli_readline.o : ../src/core/cli_readline.c
	${CC} ${CFLAGS} -o $@ $<
 
cli_shell.o : ../src/core/cli_shell.c
	${CC} ${CFLAGS} -o $@ $<
 
cli_serial.o : ../src/core/cli_serial.c
	${CC} ${CFLAGS} -o $@ $<
 
uid.o : ../src/core/uid.c
	${CC} ${CFLAGS} -o $@ $<
 
task1_start.o : ../src/apps/task1_start.c
	${CC} ${CFLAGS} -o $@ $<
 
task2_start.o : ../src/apps/task2_start.c
	${CC} ${CFLAGS} -o $@ $<
 
task3_start.o : ../src/apps/task3_start.c
	${CC} ${CFLAGS} -o $@ $<

block_init.o : ../src/block_init.s
	${ASSEMBLER} ${ASFLAGS} -o $@ $<
	
clean :
	rm -rf *.o
	rm -rf ../image/*
	rm -rf *.map

