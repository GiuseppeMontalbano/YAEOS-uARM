CC = arm-none-eabi-gcc
LINK = arm-none-eabi-gcc -nostartfiles
SRCDIRGTK = /usr/include/uarm
OPTIMIZE = -O3 -s -fomit-frame-pointer
CFLAGS = $(DEFINES) $(OPTIMIZE)
LFLAGS = -mcpu=arm7tdmi
OPT = -std=c11
PROGS = phase1

PROGS_O = pcb.o asl.o

PROGS_C = pcb.c asl.c const.h

all:  .c.o libs

progs :
	$(CC) $(LFLAGS) -c -o $(PROGS_O) $(PROGS_C)

objs:
	$(PROGS_O)

.c.o:
	mkdir build/
	$(CC) -c $(LFLAGS) -I /usr/include/uarm/ pcb.c -o build/pcb.o
	$(CC) -c $(LFLAGS) -I /usr/include/uarm/ asl.c -o build/asl.o
	$(CC) -c $(LFLAGS) -I /usr/include/uarm/ p1test.c -o build/p1test.o

.o:
	$(CC) $(LFLAGS) -o $* $(PROGS_O)

libs:
	mkdir dist/
	$(LINK) -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o build/pcb.o build/asl.o build/p1test.o -o dist/phase1

clean: cleanbin dir
	#rm -f *.o *~

cleanbin:
	rm -f dist/$(PROGS)

dir:
	rm -f build/*.o *~
	rmdir build/
	#rm -f dist/phase1
	rmdir dist/

tar:
	tar czvf lso2018az01.tgz *
