#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int sd = running->syscall_args[0];
  
  SemDescriptor* sfd = SemDescriptorList_byFd(&running->sem_descriptors,sd); //RICERCO IL DESCRITTORE DEL SEMAFORO NELLA LISTA DEL PCB
  
  //VERIFICO CHE SIA PRESENTE, ALTRIMENTI TERMINO
  if(!sfd){
	  printf("ERRORE: Il file descriptor %d non esiste\n",sd);
	  return;
  }
  (sfd->semaphore)->count--;
  
  //VERIFICO SE IL CONTATORE DEL SEMAFORO SCENDE SOTTO LO ZERO
  if((sfd->semaphore)->count < 0){
	  
	  //SPOSTO IL SUO DESCRITTORE NELLA LISTA DI WAITING
	  List_detach(&sfd->semaphore->descriptors,(ListItem*)sfd->ptr);
	  List_insert(&(sfd->semaphore)->waiting_descriptors,(sfd->semaphore)->waiting_descriptors.last,(ListItem*)sfd->ptr);
  
	  //IMPOSTO LO STATO DEL PCB DEL PROCESSO IN WAITING
	  PCB* wait_PCB = running;
	  wait_PCB->status = 0x3;
	  
	  //SPOSTO IL PCB DEL PROCESSO NELLA WAITING QUEUE DEL SISTEMA OPERATIVO
	  List_detach(&ready_list,(ListItem*)wait_PCB);
	  List_insert(&waiting_list,waiting_list.last,(ListItem*)wait_PCB);
	  
	  //METTO IN ESECUZIONE UN NUOVO PROCESSO (IL PRIMO DELLA READY QUEUE)
	  PCB* next_PCB = (PCB*)List_detach(&ready_list,(ListItem*)ready_list.first);
	  running = next_PCB;
  }
  
  running->syscall_retvalue = 0; //ASSEGNO UN VALORE DI RITORNO ALLA SYSCALL
  return;
}
