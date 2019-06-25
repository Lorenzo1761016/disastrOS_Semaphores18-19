#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  int sd = running->syscall_args[0];
  
  SemDescriptor* sfd = SemDescriptorList_byFd(&running->sem_descriptors,sd); //RICERCO IL DESCRITTORE DEL SEMAFORO NELLA LISTA DEL PCB
  
  //VERIFICO CHE SIA PRESENTE, ALTRIMENTI TERMINO
  if(!sfd){
	  printf("ERRORE: Il semaforo %d non esiste!\n",sd);
	  running->syscall_retvalue = -1;
	  return;
  }
  
  (sfd->semaphore)->count++;  //INCREMENTO IL SUO CONTATORE
  
  //SE IL CONTATORE DEL SEMAFORO E' MINORE DI SPOSTO IL SUO DESCRITTORE NELLA LISTA DI READY
  if((sfd->semaphore)->count < 0){
	  SemDescriptorPtr* ptr_aux = (SemDescriptorPtr*)List_detach(&(sfd->semaphore)->waiting_descriptors,(sfd->semaphore)->waiting_descriptors.first);
	  List_insert(&(sfd->semaphore)->descriptors,(sfd->semaphore)->descriptors.last,(ListItem*)ptr_aux);

	  //IMPOSTO LO STATO DEL PCB DEL PROCESSO IN READY
	  PCB* exec_PCB = ptr_aux->descriptor->pcb;
	  exec_PCB->status = 0x2;
	  
	  //SPOSTO IL PCB DEL PROCESSO NELLA READY QUEUE DEL SISTEMA OPERATIVO
	  List_detach(&waiting_list,(ListItem*)exec_PCB);
	  List_insert(&ready_list,ready_list.last,(ListItem*)exec_PCB);
  }
  //SETTO IL VALORE DI RITORNO DELLA SYSCALL
  running->syscall_retvalue = 0;
  return;
  
}
