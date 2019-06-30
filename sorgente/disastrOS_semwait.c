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
	  running->syscall_retvalue=-1;
	  return;
  }
  
  printf("[WAIT] Processo: %d - Semaforo: %d - Valore: From %d to %d\n", disastrOS_getpid(),(sfd->semaphore)->id, (sfd->semaphore)->count,(sfd->semaphore)->count-1);
  (sfd->semaphore)->count--;
  
  
  //VERIFICO SE IL CONTATORE DEL SEMAFORO SCENDE SOTTO LO ZERO
  if((sfd->semaphore)->count < 0){
	  
	  //SPOSTO IL SUO DESCRITTORE NELLA LISTA DI WAITING
	  SemDescriptorPtr* aux = (SemDescriptorPtr*)List_detach(&sfd->semaphore->descriptors,(ListItem*)sfd->ptr);
	  if(!aux){
		  printf("ERRORE: rimozione del puntatore a descrittore dalla lista dei descrittori del semaforo fallita\n");
		  running->syscall_retvalue=-1;
	  }
	  
	  aux = (SemDescriptorPtr*)List_insert(&(sfd->semaphore)->waiting_descriptors,(sfd->semaphore)->waiting_descriptors.last,(ListItem*)sfd->ptr);
	  if(!aux){
		  printf("ERRORE: inserimento del puntatore a descrittore nella lista di waiting del semaforo fallito\n");
		  running->syscall_retvalue=-1;
		  return;
	  }
	  //IMPOSTO LO STATO DEL PCB DEL PROCESSO IN WAITING
	  running->status = Waiting;
	  
	  //SPOSTO IL PCB DEL PROCESSO NELLA WAITING QUEUE DEL SISTEMA OPERATIVO
	  PCB* pcb_aux = (PCB*)List_insert(&waiting_list,waiting_list.last,(ListItem*)running);
	  if(!pcb_aux){
		  printf("ERRORE: inserimento del processo nella lista di attesa del sistema fallito\n");
		  running->syscall_retvalue=-1;
		  return;
	  }
	  
	  //METTO IN ESECUZIONE UN NUOVO PROCESSO (IL PRIMO DELLA READY QUEUE)
	  pcb_aux = (PCB*)List_detach(&ready_list,(ListItem*)ready_list.first);
	  if(!pcb_aux){
		  printf("ERRORE: rimozione del processo dalla ready queue del sistema fallita\n");
		  running->syscall_retvalue=-1;
		  return;
	  }
	  running = pcb_aux;
  }
  
  running->syscall_retvalue = 0; //ASSEGNO UN VALORE DI RITORNO ALLA SYSCALL
  return;
}
