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
	  List_detach(&(sfd->semaphore)->waiting_descriptors,(sfd->semaphore)->waiting_descriptors.last);
	  List_insert(&(sfd->semaphore)->descriptors,(sfd->semaphore)->descriptors.last,(sfd->semaphore)->waiting_descriptors.last);
  }
  
  //SETTO IL VALORE DI RITORNO DELLA SYSCALL
  running->syscall_retvalue = 0;
  return;
  
}
