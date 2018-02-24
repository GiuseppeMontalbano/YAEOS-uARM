#include "pcb.h"


typedef struct semd_t {
	struct semd_t *s_next;
	int *s_key;
	struct pcb_t *s_procQ;
} semd_t;


semd_t semd_table[MAXSEMD];
semd_t *semdFree_h;
semd_t *semdhash[ASHDSIZE];



int hashFun(int *key) {
    int x = (int) key;				/* Intero che mantiene tutte le modifiche ai bit della chiave */
    x = ((x >> 16)^x)*0x45d9f3b;	/* Costante moltiplicativa che massimizza l'influenza del singolo bit dell'input nell'intero risultante nell'output */
    x = ((x >> 16)^x)*0x45d9f3b;
    x = ((x >> 16)^x)*0x45d9f3b;
    
    x = x % ASHDSIZE;
    
    if(x < 0) x = -x;
    
    return x;
}


/*	-- cercaInListaDiTrabocco(semd_t *head, int *key) --
	
	Cerca il semd_t con identificativo key nella lista puntata da head.
	Ritorna il puntatore a tale semd_t se viene trovato nella lista.
	Ritorna NULL altrimenti.
*/
semd_t *cercaInListaDiTrabocco(semd_t *head, int *key) {
    if(head==NULL)
      	return NULL;
	
	if(head->s_key == key) {
      	return head;
    }
    
    cercaInListaDiTrabocco(head->s_next, key);
}


/*	-- insertInListaDiTrabocco(semd_t *sem, int *i) --
	
	Inserisce sem in testa alla lista di indice i nella semdhash.
*/
void insertInListaDiTrabocco(semd_t *sem, int *i){
	if(sem != NULL){
		if(semdhash[*i] == NULL){
			semdhash[*i] = sem;
		}
		else {
			sem->s_next = semdhash[*i];
			semdhash[*i] = sem;
		}
	}	
}


/*	-- initASL_recursive(int *i) --
	
	Inizializza la lista dei semd_t liberi scorrendo la semd_table.
	L'intero i serve a scorrere l'intera semd_table senza uscire dalla sua dimensione MAXSEMD.
*/
void initASL_recursive(int *i){
	if((*i) < MAXSEMD - 1){
		if((*i)== 0) {
			semdFree_h = &semd_table[*i];
			semd_table[*i].s_next = &semd_table[(*i) + 1];	
		}
		else {
			semd_table[*i].s_next = &semd_table[(*i) + 1]; 	
		}
		(*i) = (*i) + 1;
		initASL_recursive(i);
	}
}


/*	-- insertInSemdFree(semd_t *sem) --
	
	Inserisce il semd_t puntato da sem nella lista puntata da semdFree_h (inserimento in testa).
*/
void insertInSemdFree(semd_t *sem){
	if(sem != NULL){
		if(semdFree_h == NULL){
			semdFree_h = sem;
		}
		else {
			sem->s_next = semdFree_h;
			semdFree_h = sem;
		}
	}
}


/*	-- removeFromSemdFree() --

	Rimuove e ritorna la testa della lista dei semd liberi.
	Se la lista è vuota ritorna NULL.
*/
semd_t *removeFromSemdFree(){
	if(semdFree_h != NULL){ 	/* Se la lista dei semd liberi non è vuota usa il semd in testa */
			semd_t *tmp;		/* Puntatore al semd rimosso dalla lista dei semd liberi */
			tmp = semdFree_h;
			semdFree_h = semdFree_h->s_next;
			return tmp;
	}
	else return NULL;
		
}


/* 	-- removeDaTrabocco(semd_t **head, semd_t *sem) --
	
	Rimuove il semd_t puntato da sem dalla lista con testa head.
	Ritorna NULL se la testa non punta a nessuna lista o se sem non è presente nella lista.
	Ritorna sem se è stato trovato e rimosso dalla lista.
*/
semd_t *removeDaTrabocco(semd_t **head, semd_t *sem){
	if((*head)== NULL){
		return NULL;
	}

	else if((*head) == sem){
		(*head)=(*head)->s_next;
		sem->s_next = NULL;
		return sem;
	}

	else{
		semd_t **tmp;
		semd_t *tmp1;
		tmp = &tmp1;
		tmp1 = (*head)->s_next;
		
		if(removeDaTrabocco(tmp,sem) == sem){
			(*head)->s_next = *tmp;
			
			return sem;
		}
	
		return NULL;
	}
} 


/*    FUNZIONI PHASE 1    */


/*	-- insertBlocked(int *key, pcb_t *p) --
	
	Viene inserito il PCB puntato da  p nella coda
	dei processi  bloccati  associata  al semaforo con chiave  key.
	Se il semaforo corrispondente non è  presente nella ASHT,
 	alloca un nuovo SEMD dalla lista di quelli liberi e lo inserisce
	nella  ASHT, settando i campi in maniera opportuna. Se non
	è possibile allocare un nuovo SEMD perché la  lista  di quelli
	liberi e’ vuota, restituisce -1. In  tutti  gli altri  casi,  restituisce 0.
*/
int insertBlocked(int *key, pcb_t *p){
	semd_t *trovato; 	/* Puntatore al semd con chiave key se trovato, NULL altrimenti */
	trovato = cercaInListaDiTrabocco(semdhash[hashFun(key)], key);
	
	if(trovato != NULL){
		insertProcQ((&trovato->s_procQ), p);	/* Pcb p inserito per priorità */
		p->p_semKey = trovato->s_key;
	}

	else {
		if(semdFree_h != NULL){ 	/* Se la lista dei semd liberi non è vuota usa il semd in testa */
			semd_t *sem;
			sem = removeFromSemdFree();
			
			/* Init semd rimosso dalla lista dei semd liberi */
			sem->s_next = NULL;
			sem->s_key = key;
			sem->s_procQ = p;
			p->p_semKey = sem->s_key;

			int i = hashFun(sem->s_key);
			insertInListaDiTrabocco(sem, &i);
		}
		else return -1;
	}
	
	return 0;
}


/*	-- headBlocked(int *key) --
	
	Restituisce il puntatore al pcb del primo
	processo bloccato sul semaforo, senza deaccordarlo. Se il
	semaforo non esiste restituisce NULL.
*/
pcb_t *headBlocked(int *key){
	semd_t *trovato; 	/* Punta al semd con chiave key */
	trovato = cercaInListaDiTrabocco(semdhash[hashFun(key)], key);
	if(trovato != NULL){
		pcb_t *pcb;		/* Puntatore al primo pcb bloccato sul semd key */
		pcb = headProcQ(trovato->s_procQ);
		if(pcb != NULL) return(pcb);
	}
	return NULL;
}


/*	-- initASL() --
	
	Inizializza la lista dei semdFree in
	modo da contenere tutti gli elementi della
	semdTable. Questo metodo viene invocato una
	volta sola durante l’inizializzazione della struttura
	dati.
*/
void initASL(){
	int i = 0;	/* Counter per non finire fuori dall'array */
	initASL_recursive(&i); 
}


/*	-- removeBlocked(int *key) --
	
	Ritorna il primo PCB dalla coda dei processi
	bloccati (s_ProcQ) associata al SEMD della ASHT con chiave key.
	Se tale descrittore non esiste nella ASHT, restituisce NULL.
	Altrimenti, restituisce l’elemento  rimosso. Se la coda dei processi
	bloccati per il semaforo diventa vuota, rimuove il descrittore
	corrispondente dalla  ASHT e lo inserisce nella coda dei
	descrittori liberi (semdFree).
*/
pcb_t* removeBlocked(int *key){
	semd_t *sem;	/* Puntatore al semd di chiave key nella ASHT */
	sem = cercaInListaDiTrabocco(semdhash[hashFun(key)],key);
	if(sem == NULL){
		return NULL;
	}
	else{
		pcb_t *tmp;	/* Puntatore al pcb rimosso dalla coda dei processi bloccati del semd con id key */
		tmp = removeProcQ(&(sem->s_procQ));
		
		/* Se non ci sono più processi bloccati nel semd key, rimuoverlo dalla ASHT e inserirlo nella lista dei semd liberi */
		if(sem->s_procQ == NULL){ 
			semd_t *daMettereInFree;	/* Puntatore al semd che non ha più pcb bloccati in coda */
			daMettereInFree = removeDaTrabocco(&semdhash[hashFun(key)], sem);
			insertInSemdFree(daMettereInFree);
		}
		tmp->p_semKey = NULL;
		tmp->p_next = NULL;
		return tmp; 
	}	
}


/*	-- forallBlocked(int *key, void fun(pcb_t *pcb, void *),void *arg) --
	
	Richiama la funzione fun per ogni processo bloccato sul semaforo
	identificato da key.
*/
void forallBlocked(int *key, void fun(pcb_t *pcb, void *),void *arg){
	semd_t *sem;	/* Puntatore al semd con id key nella ASHT */
	sem = cercaInListaDiTrabocco(semdhash[hashFun(key)], key);
	if(sem != NULL){
		forallProcQ(sem->s_procQ, fun, arg);
	}
}


/*	-- outChildBlocked(pcb_t *p) --
	
	Rimuove il PCB puntato da p dalla
	coda del semaforo su cui è  bloccato.
*/
pcb_t *outChildBlocked(pcb_t *p){
	if (p->p_semKey == NULL) return NULL;
	
	int *key;		/* Key del semd in cui è bloccato p */
	semd_t *sem;	/* Puntatore al semd con chiave key nella ASHT */	
	key = p->p_semKey;
	sem = cercaInListaDiTrabocco(semdhash[hashFun(key)], key);	//trovo il sem su cui è bloccato p
	if (sem == NULL){
		return NULL;
	}
	else {
		pcb_t *pcbRemoved;
		pcbRemoved = outProcQ(&(sem->s_procQ), p);
		if(sem->s_procQ == NULL){		/* Se non ci sono più processi bloccati nel semd key, rimuovere il semd key dalla ASHT */
			semd_t *daMettereInFree;	/* Puntatore al semd eliminato dalla ASHT */
			daMettereInFree = removeDaTrabocco(&semdhash[hashFun(key)], sem);
			
			insertInSemdFree(daMettereInFree);	
		}
		return pcbRemoved;
	}
}
