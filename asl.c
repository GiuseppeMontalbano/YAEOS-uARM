#include "pcb.h"
#include "asl.h"


int hashFun(int *key) {
    int x = (int) key;				/* Intero che mantiene tutte le modifiche ai bit della chiave */
    x = ((x >> 16)^x)*0x45d9f3b;	/* Costante moltiplicativa che massimizza l'influenza del singolo bit dell'input nell'intero risultante nell'output */
    x = ((x >> 16)^x)*0x45d9f3b;
    x = ((x >> 16)^x)*0x45d9f3b;
    
    x = x % ASHDSIZE;
    
    if(x < 0) x = -x;
    
    return x;
}


/* Return semd_t con chiave key nella lista puntata da head */
semd_t *cercaInListaDiTrabocco(semd_t *head, int *key) {
    if(head==NULL)
      	return NULL;
	
	if(head->s_key == key) {
      	return head;
    }
    
    cercaInListaDiTrabocco(head->s_next, key);
}



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



void initASL_recursive(int *i){
	if((*i) < MAXSEMD - 1){
		if((*i)== 0) {		
			semdFree_h = &semd_table[*i];					/* Collegamento semdFree_h al primo elemento array */
			semd_table[*i].s_next = &semd_table[(*i) + 1];	
		}
		else {
			semd_table[*i].s_next = &semd_table[(*i) + 1];	/* Collegamento elemento array al successivo con modifica campo s_next */ 	
		}
		(*i) = (*i) + 1;
		initASL_recursive(i);
	}
}



void insertInSemdFree(semd_t *sem){
	if(sem != NULL){
		if(semdFree_h == NULL){			/* Caso lista semd liberi vuota */
			semdFree_h = sem;
		}
		else {						
			sem->s_next = semdFree_h;	/* Inserimento in testa */
			semdFree_h = sem;
		}
	}
}

/* Return semd_t rimosso dalla lista puntata da semdFree_h */
semd_t *removeFromSemdFree(){
	if(semdFree_h != NULL){ 	/* Se la lista dei semd liberi non è vuota usa il semd in testa */
			semd_t *tmp;		/* Puntatore al semd rimosso dalla lista dei semd liberi */
			tmp = semdFree_h;
			semdFree_h = semdFree_h->s_next;
			return tmp;
	}
	else return NULL;
		
}


/* Return semd_t sem rimosso dalla lista puntata da head */
semd_t *removeDaTrabocco(semd_t **head, semd_t *sem){
	if((*head)== NULL){			/* Lista vuota o sem non trovato */
		return NULL;
	}

	else if((*head) == sem){	/* Sem trovato */
		(*head)=(*head)->s_next;
		sem->s_next = NULL;
		return sem;
	}

	else{						/* Ricorsione */
		semd_t **tmp;			/* Puntatore utilizzato per scorrere la coda. *tmp punta sempre all'elemento successivo di *head. Nel caso in cui *tmp è proprio l'elemento da eliminare allore *tmp punta ad *head->next->next */
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



void initASL(){
	int i = 0;	/* Counter per non finire fuori dall'array */
	initASL_recursive(&i); 
}



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



void forallBlocked(int *key, void fun(pcb_t *pcb, void *),void *arg){
	semd_t *sem;	/* Puntatore al semd con id key nella ASHT */
	sem = cercaInListaDiTrabocco(semdhash[hashFun(key)], key);
	if(sem != NULL){
		forallProcQ(sem->s_procQ, fun, arg);
	}
}


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
