#include <stdlib.h>
#include <stdio.h>

#include "pcb.h"

/**
 * Inizializza la pcbFree in modo da contenere
 * tutti gli elementi della pcbFree_table. 
 */
void initPcbs();
/**
 * Inserisce il PCB puntato da p nella lista 
 * dei PCB liberi (pcbFree)
 */
void freePcb(pcb_t *p);
/**
 * Restituisce NULL se la pcbFree è vuota. 
 * Altrimenti rimuove un elemento dalla pcbFree, 
 * inizializza tutti i campi (NULL/0)
 * e restituisce l’elemento rimosso.
 */
pcb_t *allocPcb();
/**
 * richiama la funzione fun per ogni elemento della lista puntata
 * da head
 */
void forAllProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg) {
   if(head != NULL) {
      fun(head, arg);
      forAllProcQ(head->p_next, fun, arg);
    }
}
/**
 * Rimuove il PCB puntato da p dalla coda dei
 * processi puntata da head. Se p non è presente nella coda,
 * restituisce NULL. (NOTA: p può trovarsi in una posizione
 * arbitraria all’interno della coda)
 */
pcb_t *outProcQ(pcb_t **head, pcb_t *p){	    	
   if(*head == NULL) {	//caso coda vuota o elemento non trovato
      return NULL;
   } else if(p == (*head)) {	//caso in cui l'elemento da eliminare è presente nella coda
      (*head) = (*head)->p_next;
      p->p_next = NULL;
      return p;
   } else {
      //tmp è un puntatore utilizzato per scorrere la coda. *tmp punta sempre all'elemento successivo di
      //*head. Nel caso in cui *tmp è proprio l'elemento da eliminare allore *tmp punta ad *head->next->next
      pcb_t **tmp=malloc(sizeof(pcb_t*));
      *tmp=(*head)->p_next;									
      if(outProcQ(tmp,p) == p) {
         (*head)->p_next=*tmp;
         free(tmp);
         return p;        
      }    
      free(tmp);
      return NULL;            
   }      
}
/**
 * Restituisce l’elemento di testa della coda
 * dei processi da head,  SENZA RIMUOVERLO. Ritorna
 * NULL se la coda non ha elementi.
 */
pcb_t *headProcQ(pcb_t *head) {
   if(head == NULL) return NULL;
   else return head;
}
/**
 * rimuove il primo elemento dalla coda dei
 * processi puntata da head. Ritorna NULL se la coda è  vuota.
 * Altrimenti ritorna il  puntatore  all’elemento rimosso dalla lista
 */
pcb_t *removeProcQ(pcb_t **head) {
   if (*head==NULL) {
       return NULL ;
   } else {
      pcb_t *tmp=*head;	//punta all'elemento che sarà rimosso
      *head=(*head)->p_next;
      tmp->p_next=NULL;
      return tmp;
   }
}

/**
 * inserisce l’elemento puntato da p nella  
 * coda dei processi puntata da head. L’inserimento deve
 * avvenire tenendo conto della priorità di ciascun pcb  
 * (campo p‐>priority). La coda dei processi deve essere
 * ordinata in base alla priorità dei PCB, in ordine
 * decrescente (i.e. l’elemento di testa è l’elemento con la
 * priorità più alta)
 */
void insertProcQ(pcb_t** head, pcb_t* p){
   if(*head==NULL){	//coda vuota,p è il primo elemento ad essere inserito			
       *head=p;
   } else if(p->p_priority >= (*head)->p_priority) {	//caso in cui la priorità dell'elemento da inserire è più alta del pcb considerato
      p->p_next=*head;
      *head=p;          
   } else {
      //puntatore temporaneo utilizzato per scorrere la lista.
      //*tmp punta sempre all'elemento successivo di *head.
      pcb_t** tmp=malloc(sizeof(pcb_t*));	 
      *tmp = (*head)->p_next;			 
      if(*tmp!=NULL){
         insertProcQ(tmp,p);
         if (p->p_next==(*head)->p_next) (*head)->p_next=*tmp;  //aggiorno il successore di *head, con il nuovo successore *tmp
      } else {
         (*head)->p_next=p;			//caso in cui siamo arrivati in fondo alla coda , quindi p sarà l'ultimo elemento della coda
      }
      free(tmp);
   }
}
/**
 * Inserisce il PCB puntato da p come figlio del PCB
 * puntato da parent.
 */
void insertChild(pcb_t *parent, pcb_t *p) {
   pcb_t *fc = parent->p_first_child;
   if(fc->p_sib == NULL) {
      fc->p_sib = p;
      p->p_parent = parent;
   } else insertSib(fc->p_sib, p);
   return;
}
/**
 * Rimuove il primo figlio del PCB puntato da p. Se
 * p non ha figli, restituisce NULL
 */
pcb_t *removeChild(pcb_t *p) {
   pcb_t *tmp;
   if(p->p_first_child == NULL) {
      return NULL;
   } else {
      tmp = p->p_first_child;
      p->p_first_child = tmp->p_sib;
      return tmp;
   }
}
 
/**
 * Rimuove il PCB puntato da p dalla lista dei figli
 * del padre. Se il PCB puntato da p non ha un padre, restituisce
 * NULL. Altrimenti restituisce l’elemento rimosso (cioé p). A
 * differenza della removeChild, p può trovarsi in una posizione
 * arbitraria (ossia non è necessariamente il primo figlio del padre)
 */
pcb_t *outChild(pcb_t *p) {
   pcb_t *parent = p->p_parent;
   if(parent == NULL) return NULL;
   else if(p == parent->p_first_child)
      return removeChild(parent);
   else return removeSib(p, parent->p_first_child);
}

/***************************Test Function****************************/
pcb_t *createRoot() {
   pcb_t *r = malloc(sizeof(*r));
   r->p_parent = NULL;
   r->p_first_child = NULL;
   r->p_sib = NULL;
   return r;
}
/*Inserisce p come first child */
void insertFirstChild(pcb_t *parent, pcb_t *p) {
   pcb_t *fc = parent->p_first_child;
   parent->p_first_child = p;
   p->p_sib = fc;
}
void insertSib(pcb_t *p, pcb_t *sib) {
   if(p->p_sib == NULL) {
      p->p_sib = sib;
      sib->p_parent = p->p_parent;
   } else insertSib(p->p_sib, sib);
}

pcb_t *removeSib(pcb_t *p, pcb_t *sib) {
   if(sib == NULL) return NULL;
   if(sib->p_sib == p) {
      sib->p_sib = p->p_sib;
      return p;
   } else return removeSib(p, sib->p_sib); 
}

char *printPCBTree(pcb_t *t, int i, int h) {
   if(t->p_first_child == NULL) {
      printf("(%d)%d\t", h, i);
   } else {
      printf("(%d)%d\t", h, i);
      printPCBTree(t->p_first_child, i+1, h+1);
   }
   if(t->p_sib != NULL) printPCBTree(t->p_sib, i+1, h);
}

int treeH(pcb_t *t) {
   int h = 0;
   if(t->p_first_child == NULL) return h;
   else {
      h = 1 + treeH(t->p_first_child);
      return h;
   }
}
