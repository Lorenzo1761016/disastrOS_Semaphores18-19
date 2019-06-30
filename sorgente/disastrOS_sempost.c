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
  
  printf("[POST] Processo: %d - Semaforo: %d - Valore: From %d to %d\n", disastrOS_getpid(),(sfd->semaphore)->id, (sfd->semaphore)->count, (sfd->semaphore)->count+1);
  (sfd->semaphore)->count++;  //INCREMENTO IL SUO CONTATORE
  
  
  //VERIFICO SE IL CONTATORE DEL SEMAFORO E' MINORE DI ZERO
  if((sfd->semaphore)->count <= 0){
	  
	  //SPOSTO IL SUO DESCRITTORE NELLA LISTA DI READY
	  SemDescriptorPtr* ptr_aux = (SemDescriptorPtr*)List_detach(&(sfd->semaphore)->waiting_descriptors,(sfd->semaphore)->waiting_descriptors.first);
	  if(!ptr_aux){
		  printf("ERRORE: rimozione del descrittore dalla lista di waiting del semaforo fallita\n");
		  running->syscall_retvalue = -1;
		  return;
	  }
	  SemDescriptorPtr* ret = (SemDescriptorPtr*)List_insert(&(sfd->semaphore)->descriptors,(sfd->semaphore)->descriptors.last,(ListItem*)ptr_aux);
	  if(!ret){
		  printf("ERRORE: inserimento del puntatore a descrittore nella lista dei descrittori del semaforo fallito\n");
		  running->syscall_retvalue=-1;
	  }

	  //IMPOSTO LO STATO DEL PCB DEL PROCESSO IN READY
	  PCB* exec_PCB = ptr_aux->descriptor->pcb;
	  exec_PCB->status = Ready;
	  
	  //SPOSTO IL PCB DEL PROCESSO NELLA READY QUEUE DEL SISTEMA OPERATIVO
	  PCB* pcb_aux = (PCB*)List_detach(&waiting_list,(ListItem*)exec_PCB);
	  if(!pcb_aux){
		  printf("ERRORE: rimozione del processo dalla waiting list fallita\n");
		  running->syscall_retvalue=-1;
	  }
	  
	  pcb_aux = (PCB*)List_insert(&ready_list,ready_list.last,(ListItem*)exec_PCB);
	  if(!pcb_aux){
		  printf("ERRORE: inserimento del processo nella ready queue fallito\n");
		  running->syscall_retvalue = -1;
	  }
  }
  //SETTO IL VALORE DI RITORNO DELLA SYSCALL
  running->syscall_retvalue = 0;
  return;
  
}
