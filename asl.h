#ifndef ASL_H
#define ASL_H

#include "uARMconst.h"
#include "uARMtypes.h"
#include "libuarm.h"

#include "pcb.h"
#include "const.h"



typedef struct semd_t {
   struct semd_t *s_next;
   int *s_key;
   pcb_t *s_procQ;
} semd_t;

semd_t *semdFree_h;
semd_t semd_table[MAXSEMD];
semd_t *semdhash[ASHDSIZE];


int insertBlocked(int *key, pcb_t *p);
pcb_t *headBlocked(int *key);
pcb_t *removeBlocked(int *key);
void forallBlocked(int *key, void fun(pcb_t *pcb, void *),void *arg);
pcb_t *outChildBlocked(pcb_t *p);
void initASL();

#endif //ASL_H
