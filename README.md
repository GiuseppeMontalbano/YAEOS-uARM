# YAEOS - uARM
## Phase 1 - Gestione delle Code
Il gestore delle code implementa quattro funzionalità  relative ai PCB:  
  - Allocazione e deallocazione dei PCB.
  - Gestione delle code dei  PCB.  
  - Gestione dell’abero dei  PCB.  
  - Gestone  di  una  Active  Semaphore Hash Table (ASHT), che  gestisce i processi bloccati sui semafori.
### Piattaforma
La piattaforma su cui verrà eseguito il codice è uARM (https://github.com/mellotanica/uARM).
### Compilazione
Esempio con singolo file di test helloworld.c:
```
$> arm-none-eabi-gcc -mcpu=arm7tdmi -c -o helloworld.o helloworld.c
```
Il comando crea un object file "helloworld.o" compilato per processori ARM7TDMI.
Adesso bisogna fare il link delle librerie di sistema:
```
$> arm-none-eabi-ld -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x \\
   -o helloworld /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o helloworld.o
```
Il file `elf32ltsarm.h.uarmcore.x` è uno specifico linker script per il linking di eseguibili uARM.
L'eseguibile creato è utilizzabile nella piattaforma selezionandolo nella sezione
*Machine Config -> General -> Firmware & Software -> Core File*

Per comodità è presente un `makefile` che esegue il codice sopra per tutti i file necessari.

### Test
Il file `p1test.c` è il test di riferimento per verificare la correttezza del codice prodotto.
Il `makefile` è già disposto per creare un eseguibile compatibile con uARM chiamato `phase1`.
Basta selezionare l'eseguibile in *[...] -> Core File* e lanciare la macchina.
Gli output sono visibili nel _terminal 0_.

Il file di test è puramente funzionale. Verifica la correttezza del codice, quindi non è necessario fare il cross-compiling.
Compilare il file `test.c` ed eseguirlo.
