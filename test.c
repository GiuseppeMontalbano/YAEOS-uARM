#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pcb.h"

void pcb_queue_section() {
   printf("\n/******************** PCB Queue Test ********************/\n\n");
   pcb_t* elem=malloc(sizeof(pcb_t));
   pcb_t* elem2=malloc(sizeof(pcb_t));
   pcb_t* elem3=malloc(sizeof(pcb_t));
   pcb_t* elem4=malloc(sizeof(pcb_t));
   pcb_t* elem5=malloc(sizeof(pcb_t));

   pcb_t** head=malloc(sizeof(pcb_t*));
   *head=NULL;
    
   elem->p_priority=2;
   elem->p_next=NULL;
   insertProcQ(head,elem);
    
   elem2->p_next=NULL;
   elem2->p_priority=5;
   insertProcQ(head,elem2);
    
   elem3->p_priority=7;
   elem3->p_next=NULL;
   insertProcQ(head,elem3);
    
   elem4->p_priority=8;
   elem4->p_next=NULL;
   insertProcQ(head,elem4);
    
   elem5->p_priority=1;
   elem5->p_next=NULL;
   insertProcQ(head,elem5);

   printf("\nTest insertProcQ:\t");
   printf("%d\t",(*head)->p_priority);
   printf("%d\t",(*head)->p_next->p_priority);
   printf("%d\t",(*head)->p_next->p_next->p_priority);
   printf("%d\t",(*head)->p_next->p_next->p_next->p_priority);
   printf("%d\n",(*head)->p_next->p_next->p_next->p_next->p_priority);

   pcb_t *b=headProcQ(*head);
   printf("\nTest headProcQ:\t");
   printf("%d\n",b->p_priority);

   pcb_t* a=outProcQ(head,elem4);
   printf("\nTest outProcQ:\t");
   printf("%d\t",(*head)->p_priority);
   printf("%d\t",(*head)->p_next->p_priority);
   printf("%d\t",(*head)->p_next->p_next->p_priority);
   printf("%d\n",(*head)->p_next->p_next->p_next->p_priority);

   b=headProcQ(*head);
   printf("\nRe-test headProcQ:\t");
   printf("%d\n",b->p_priority);
   
   if (a == NULL) printf("Elemento non c'è");
   else printf("%d", a->p_priority);
}

void pcb_tree_section() {
   printf("\n/******************** PCB Tree Test ********************/\n\n");
   printf("Creating PCB root...\n");
   pcb_t *root = createRoot();

   pcb_t *a = createRoot();
   insertFirstChild(root, a);
   pcb_t *b = createRoot();
   insertChild(root,b);

   if(root->p_first_child == a)
      printf("Test:insertFirstChild(): OK\n");
   else printf("Test:insertFirstChild(): ERROR\n");
   if(a->p_sib == b)
      printf("Test:insertChild(): OK\n");
   else printf("Test:insertChild(): ERROR\n");

   pcb_t *c = createRoot();
   insertFirstChild(a, c);

   printf("Printing PCB tree (height level)<value>\\t:\n");
   printPCBTree(root,0,0);
   printf("\n");
   printf("Tree height: %d\n", treeH(root));

   pcb_t *r = removeChild(a);
   if(r == c) printf("Test:removeChild()[part 1]: OK\n");
   else printf("Test:removeChild()[part 1]: ERROR\n");
   r = removeChild(a);
   if(r == NULL) printf("Test:removeChild()[part 2]: OK\n");
   else printf("Test:removeChild()[part 2]: ERROR\n");
   printPCBTree(root,0,0);
   printf("\n");

   pcb_t *o = outChild(b);
   if(o == b) printf("Test:outChild()[part 1]: OK\n");
   else printf("Test:outChild()[part 1]: ERROR\n");

   printPCBTree(root,0,0);
   printf("\n");

   printf("\n...Done\n");
}

int main(int argc, char** argv) {
   if(argc <= 1) {
      printf("test: error: no input option.\n\nUsage:\n\n\t./test [options]\n\nOptions:\n\n\t-q\tpcb queue test.\n\t-t\tpcb tree test.\n\t-help\tprint this view.\n\n");
      //break;
   } else if(strcmp(argv[1],"-q")==0) {
      pcb_queue_section();
   } else if(strcmp(argv[1],"-t")==0) {
      pcb_tree_section();
   } else if(strcmp(argv[1],"-help")==0) {
      printf("\n\nUsage:\n\n\t./test [options]\n\nOptions:\n\n\t-q\tpcb queue test.\n\t-t\tpcb tree test.\n\t-help\tprint this view.\n\n");
   } else {
      printf("\n\nUsage:\n\n\t./test [options]\n\nOptions:\n\n\t-q\tpcb queue test.\n\t-t\tpcb tree test.\n\t-help\tprint this view.\n\n");
      //break;
   }
   return 0;
}
