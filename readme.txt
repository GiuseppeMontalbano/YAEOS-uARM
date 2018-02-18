Sistemi Operativi - uARM

Sviluppo del progetto YAEOS.

1.0 Compilazione per uARM
2.0 Phase 1: Gestione delle code


1.0 ****************************************************

  Esempio con file test helloWorld.c:
  
$> arm-none-eabi-gcc -mcpu=arm7tdmi -c -o helloWorld.o helloWorld.c

  Il comando crea un object file "helloWorld.o" compilato per processori ARM7TDMI.
  Adesso bisogna fare il link delle librerie di sistema:

$> arm-none-eabi-ld -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o helloWorld /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o helloWorld.o

  Il file "elf32ltsarm.h.uarmcore.x" è uno specifico linker script per il linking di eseguibili uARM.
