#include <uARMconst.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include "const.h"

typedef struct semd_t {
   struct semd_t *s_next;
   int *s_key;
   struct pcb_t *s_procQ;
} semd_t;

semd_t *semdfree_h, semd_table[MAXSEMD], *semdhash[ASHDSIZE];

int insertBlocked(int *key, pcb_t *p);
pcb_t *headBlocked(int *key);
pcb_t *removeBlocked(int *key);
void forallBlocked(int *key, void fun(pcb_t *pcb, void *),void *arg);
void outChildBlocked(pcb_t *p);
void initASL();
