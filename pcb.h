#ifndef PCB_H
#define PCB_H

#include "uARMconst.h"
#include "uARMtypes.h"
#include "libuarm.h"

#include "const.h"


typedef struct pcb_t {
   struct pcb_t *p_next;
   struct pcb_t *p_parent;
   struct pcb_t *p_first_child;
   struct pcb_t *p_sib;
   state_t p_s;
   int p_priority;
   int *p_semKey;
} pcb_t;

pcb_t *pcbfree_h, pcbfree_table[MAXPROC];

void initPcbs();
void freePcb(pcb_t *p);
pcb_t *allocPcb();

void insertProcQ(pcb_t **head, pcb_t *p);
pcb_t *headProcQ(pcb_t *head);
pcb_t *removeProcQ(pcb_t **head);
pcb_t *outProcQ(pcb_t **head, pcb_t *p);
void forallProcQ(pcb_t *head, void fun(pcb_t *pcb, void *), void *arg);

void insertChild(pcb_t *parent, pcb_t *p);
pcb_t *removeChild(pcb_t *p);
pcb_t *outChild(pcb_t *p);

void insertSib(pcb_t *p, pcb_t *sib);
pcb_t *removeSib(pcb_t *p, pcb_t *sib);

/***************************Test Function****************************/
pcb_t *createRoot();
void insertFirstChild(pcb_t *parent, pcb_t *p);
char *printPCBTree(pcb_t *t, int i, int h);
int treeH(pcb_t *t);


#endif

