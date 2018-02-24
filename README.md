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
Per compilare eseguire i seguenti comandi nella directory in cui è presente l'archivio tar "lso2018az01.tar.gz":

$> mkdir lso2018az01
$> cd lso2018az01
$> mv ../lso2018az01.tar.gz .
$> tar -xzvf lso2018az01.tar.gz
$> make

L'eseguibile creato è utilizzabile nella piattaforma selezionandolo nella sezione
*Machine Config -> General -> Firmware & Software -> Core File*

### Test
Il file `p1test.c` è il test di riferimento per verificare la correttezza del codice prodotto.
Il `makefile` è già disposto per creare un eseguibile compatibile con uARM chiamato `phase1`.
Basta selezionare l'eseguibile in *[...] -> Core File* e lanciare la macchina.
Gli output sono visibili nel _terminal 0_.

Il file di test è puramente funzionale. Verifica la correttezza del codice, quindi non è necessario fare il cross-compiling.
