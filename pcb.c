#include "pcb.h"
/******************** FEDERICO ********************/
//inizializza tutti i campi di p a NULL o 0
void initPcbNULL(pcb_t *p){
  p->p_first_child=NULL;
  p->p_next=NULL;
  p->p_parent=NULL;
  p->p_s.a1=0;
  p->p_s.a2=0;
  p->p_s.a3=0;
  p->p_s.a4=0;
  p->p_s.v1=0;
  p->p_s.v2=0;
  p->p_s.v3=0;
  p->p_s.v4=0;
  p->p_s.v5=0;
  p->p_s.v6=0;
  p->p_s.sl=0;
  p->p_s.fp=0;
  p->p_s.ip=0;
  p->p_s.sp=0;
  p->p_s.lr=0;
  p->p_s.pc=0;
  p->p_s.cpsr=0;
  p->p_s.CP15_Control=0;
  p->p_s.CP15_EntryHi=0;
  p->p_s.CP15_Cause=0;
  p->p_s.TOD_Hi=0;
  p->p_s.TOD_Low=0;
  p->p_sib=NULL;
  p->p_priority=0;
  p->p_semKey=0;
}
void CreaLista(pcb_t **head,int i){
  if (i<MAXPROC){
    *head=&pcbfree_table[i]; //fa si che il valore del puntatore puntato da head sia uguale all'elemento i dell'array
    CreaLista(&((*head)->p_next),i+1); //passo ricorsivo,richiama CreaLista con i valori successivi
  }
  else
  *head=NULL; // infine temrina la lista ponendo NULL il next dell'ultimo elemento
}
/*
* Inizializza la pcbFree in modo da contenere
* tutti gli elementi della pcbFree_table.
*/
void initPcbs(){
  int i=0;
  CreaLista(&pcbfree_h,i);
}
/*
* Inserisce il PCB puntato da p nella lista
* dei PCB liberi (pcbFree)
*/
void freePcb(pcb_t *p){
  pcb_t *temp=pcbfree_h;
  pcbfree_h=p; //inserisce p in testa alla lista dei pcb
  pcbfree_h->p_next=temp; //riaggiorna il valore della head alla nuva testa
}
/*
* Restituisce NULL se la pcbFree è vuota.
* Altrimenti rimuove un elemento dalla pcbFree,
* inizializza tutti i campi (NULL/0)
* e restituisce l’elemento rimosso.
*/
pcb_t *allocPcb(){
  if (pcbfree_h==NULL) //se la lista è vuota torna NULL
  return NULL;
  else{ //altrimenti aggiorna la testa della lista al secondo elemento,rimuove il nuovo elemento in testa,
    pcb_t *p; //inizializza l'elemento con initPcbNULL e lo ritorna
    p=pcbfree_h;
    pcbfree_h=pcbfree_h->p_next;
    initPcbNULL(p);
    return p;
  }
}
/******************** VALERIO ********************/
void forallProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg){
  if(head!=NULL) {
    fun(head,arg);
    forallProcQ(head->p_next, fun , arg);
  }
}
/*
 *
 */
pcb_t *outProcQ(pcb_t **head, pcb_t *p){
  if(*head==NULL) { //caso coda vuota o elemento non trovato
    return NULL;
  }
  else if(p == (*head)){ //caso in cui l'elemento da eliminare è presente nella coda
  (*head)=(*head)->p_next;
  p->p_next=NULL;
  return p;
  }
  else{
    pcb_t **tmp; //tmp è un puntatore utilizzato per scorrere la coda. *tmp punta sempre all'elemento successivo di
    pcb_t *tmp1; //*head. Nel caso in cui *tmp è proprio l'elemento da eliminare allore *tmp punta ad *head->next->next
    tmp=&tmp1;
    *tmp=(*head)->p_next;
    if(outProcQ(tmp,p)==p) {
      (*head)->p_next=*tmp;
      return p;
    }
    return NULL;
  }
}
/*
 *
 */
pcb_t *headProcQ(pcb_t *head){
  if(head==NULL){
    return NULL;
  }
  else return head;
}
/*
 *
 */
pcb_t *removeProcQ(pcb_t **head){
  if (*head==NULL){
    return NULL ;
  }
  else{
    pcb_t *tmp=*head; //punta all'elemento che sarà rimosso
    *head=(*head)->p_next;
    tmp->p_next=NULL;
    return tmp;
  }
}
/*
 *
 */
void insertProcQ(pcb_t** head, pcb_t* p){
  if(*head==NULL){ //coda vuota,p è il primo elemento ad essere inserito
    *head=p;
  }
  else if(p->p_priority >= (*head)->p_priority){ //caso in cui la priorità dell'elemento da inserire è più alta del pcb considerato
    p->p_next=*head;
    *head=p;
  }
  else{
    pcb_t** tmp; //puntatore temporaneo utilizzato per scorrere la lista.*tmp punta sempre all'elemento successivo di *head.
    pcb_t* tmp1;
    tmp=&tmp1;
    *tmp=(*head)->p_next;
    if(*tmp!=NULL){
      insertProcQ(tmp,p);
      if (p->p_next==(*head)->p_next) (*head)->p_next=*tmp; //aggiorno il successore di *head, con il nuovo successore *tmp
    }
    else{
      (*head)->p_next=p; //caso in cui siamo arrivati in fondo alla coda , quindi p sarà l'ultimo elemento della coda
    }
  }
}
/******************** GIUSEPPE ********************/
/**
* Inserisce p come fratello piu' a destra
* della lista dei fratelli puntata da sib.
*/
void insertSib(pcb_t *p, pcb_t *sib) {
  if(p->p_sib == NULL) {	//arrivati all'ultimo fratello
    p->p_sib = sib;
    sib->p_parent = p->p_parent;
  } else {
    insertSib(p->p_sib, sib);	//scorro la lista
  }
}
/**
* Inserisce il PCB puntato da p come figlio del PCB
* puntato da parent. Se parent ha dei figli p verrà
* inserito come ultimo figlio, il fratello più a destra.
*/
void insertChild(pcb_t *parent, pcb_t *p) {
  pcb_t *fc = parent->p_first_child;
  if(fc == NULL) parent->p_first_child = p; //primo figlio
  else if(fc->p_sib == NULL) { //secondo figlio
    fc->p_sib = p;
    p->p_parent = parent;
  } else insertSib(fc->p_sib, p); //p ultimo figlio
}
/**
* Rimuove il primo figlio del PCB puntato da p. Se
* p non ha figli, restituisce NULL
*
* @return pcb_t * il primo figlio rimosso, altrimenti NULL.
*/
pcb_t *removeChild(pcb_t *p) {
  pcb_t *tmp;
  if(p->p_first_child == NULL) {
    return NULL;
  } else {
    tmp = p->p_first_child;
    p->p_first_child = tmp->p_sib;
    tmp->p_parent = NULL;
    tmp->p_sib = NULL;
    tmp->p_first_child = NULL;
    return tmp;
  }
}
/**
* Rimuove il PCB puntato da p dalla lista dei fratelli puntata da sib.
* Restituisce il PCB puntato da p se questo fa parte della lista dei fratelli
* altrimenti NULL
*
* @return pcb_t * p, altrimenti NULL.
*/
pcb_t *removeSib(pcb_t *p, pcb_t *sib) {
  if(sib == NULL) return NULL;	//fine lista
  if(sib->p_sib == p) {  //p fa parte della lista: rimuoverlo
    sib->p_sib = p->p_sib;
    p->p_parent = NULL;
    return p;
  } else return removeSib(p, sib->p_sib); //scorro la lista
}
/**
* Rimuove il PCB puntato da p dalla lista dei figli
* del padre. Se il PCB puntato da p non ha un padre, restituisce
* NULL. Altrimenti restituisce l’elemento rimosso (cioé p). A
* differenza della removeChild, p può trovarsi in una posizione
* arbitraria (ossia non è necessariamente il primo figlio del padre)
*
* @return pcb_t * p, 
*/
pcb_t *outChild(pcb_t *p) {
  pcb_t *parent = p->p_parent;
  if(parent == NULL) return NULL; //p non ha padre
  else if(p == parent->p_first_child)	//p è primo figlio
  return removeChild(parent);
  else return removeSib(p, parent->p_first_child); //p è un fratello
}
