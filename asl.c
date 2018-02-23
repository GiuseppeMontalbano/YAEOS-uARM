#include "pcb.h"

typedef struct semd_t {
	struct semd_t *s_next;
	int *s_key;
	struct pcb_t *s_procQ;
} semd_t;

semd_t semd_table[MAXSEMD];
semd_t *semdFree_h;
semd_t *semdhash[ASHDSIZE];

#if 0
//se ASHDSIZE è primo va benissimo
int hashFun(int *chiave){
int p = (int)chiave;
p = p % ASHDSIZE;
if (p < 0) {
p = -p;
}
return p;
}
#endif

#if 1
//buona distribuzione anche se ASHDSIZE non è primo, ma come funziona però?????
int hashFun(int *key) {
	int x = (int) key;
	x = ((x >> 16)^x)*0x45d9f3b;
	x = ((x >> 16)^x)*0x45d9f3b;
	x = ((x >> 16)^x)*0x45d9f3b;

	x = x%ASHDSIZE;
	if(x<0) x = -x;
	return x;
}
#endif

/* return il puntatore al semaforo con valoreDaTrovare come key, NULL se non lo trova */
semd_t *cercaInListaDiTrabocco(semd_t *l, int *valoreDaTrovare) {
	if(l==NULL) return NULL;
	if(l->s_key == valoreDaTrovare) {  //si confrontano gli indirizzi, non i valori al loro interno
		return l;
	}
	cercaInListaDiTrabocco(l->s_next, valoreDaTrovare);
}

/* inserisce sem in testa alla lista di trabocco i-esima, i per sapere quale lista di trabocco è */
void insertInListaDiTrabocco(semd_t *semDaInserire, int *i){
	if(semDaInserire != NULL){
		if(semdhash[*i] == NULL){
			semdhash[*i] = semDaInserire;
		}
		else {
			semDaInserire->s_next = semdhash[*i];
			semdhash[*i] = semDaInserire;
		}
	}
}

/* i counter */
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

void insertInSemdFree(semd_t *daMettereInFree){
	if(daMettereInFree != NULL){
		if(semdFree_h == NULL){
			semdFree_h = daMettereInFree;
		}
		else {
			daMettereInFree->s_next = semdFree_h;
			semdFree_h = daMettereInFree;
		}
	}
}

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

/*    DA QUI LE FUNZIONI     */

/* inserimento nella lista di trabocco con LIFO */
int insertBlocked(int *key, pcb_t *p){
	semd_t *trovato; /* punta al semd con chiave key se trovato, NULL altrimenti*/
	trovato = cercaInListaDiTrabocco(semdhash[hashFun(key)], key); //scorri la lista di trabocco puntata da semdhash[i]
	if(trovato != NULL){
		insertProcQ((&trovato->s_procQ), p);
	}
	else {
		//if ce ne sono ancora nella semfree, allora prendine uno e usalo
		if(semdFree_h != NULL){ /*if semdFree list is not empty togli un elemento da essa, sistemane i campi, e inseriscilo nella lista di trabocco giusta*/
			semd_t *tmp;
			tmp = semdFree_h;
			semdFree_h = semdFree_h->s_next;

			tmp->s_next = NULL;	//setta i campi
			tmp->s_key = key;
			tmp->s_procQ = p;
			p->p_semKey = tmp->s_key;

			int i = hashFun(tmp->s_key);
			insertInListaDiTrabocco(tmp, &i);
		}
		else return -1;
	}
	return 0;
}

pcb_t *headBlocked(int *key){
	int i;
	semd_t *trovato; /* punta al semd con chiave key se trovato, NULL altrimenti*/

	i = hashFun(key);
	trovato = cercaInListaDiTrabocco(semdhash[i], key); //scorri la lista di trabocco puntata da semdhash[i]
	if(trovato != NULL){
		if(headProcQ(trovato->s_procQ) != NULL) return(headProcQ(trovato->s_procQ));
	}
	return NULL;
}

void initASL(){
	int i = 0;	/* Counter per non finire fuori dall'array */
	initASL_recursive(&i); /* */
}

pcb_t* removeBlocked(int *key){
	if(cercaInListaDiTrabocco(semdhash[hashFun(key)],key) == NULL){
		return NULL;
	}
	else{
		pcb_t *tmp;
		tmp = removeProcQ(&(cercaInListaDiTrabocco(semdhash[hashFun(key)],key)->s_procQ));
		if(cercaInListaDiTrabocco(semdhash[hashFun(key)],key)->s_procQ == NULL){ //se non ci sono piu blocked in sem key
			semd_t *daMettereInFree;
			daMettereInFree = removeDaTrabocco(&semdhash[hashFun(key)], cercaInListaDiTrabocco(semdhash[hashFun(key)],key));
			//inserisco semTrovato in testa nella sem free list
			insertInSemdFree(daMettereInFree);
			return tmp;
		}
	}
}

void forallBlocked(int *key, void fun(pcb_t *pcb, void *),void *arg){
	semd_t *sem;
	sem = cercaInListaDiTrabocco(semdhash[hashFun(key)], key);//trovo il sem su cui è bloccato p
	if(sem != NULL){
		forallProcQ(sem->s_procQ, fun, arg); //quella di lerio
	}
}

void outChildBlocked(pcb_t *p){
	int *key = p->p_semKey;
	semd_t *sem;
	sem = cercaInListaDiTrabocco(semdhash[hashFun(key)], key);	//trovo il sem su cui è bloccato p
	outProcQ(&(sem->s_procQ), p); //quella di lerio
}
