#include <uARMconst.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include "const.h"

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
