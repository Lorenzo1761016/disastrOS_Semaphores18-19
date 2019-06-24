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
  
  //SE IL CONTATORE DEL SEMAFORO SCENDE SOTTO LO ZERO SPOSTO IL SUO DESCRITTORE NELLA LISTA DI WAITING
  if((sfd->semaphore)->count < 0){
	  List_detach(&sfd->semaphore->descriptors,(ListItem*)sfd->ptr);
	  List_insert(&(sfd->semaphore)->waiting_descriptors,(sfd->semaphore)->waiting_descriptors.last,(ListItem*)sfd->ptr);
  }
 
  
  running->syscall_retvalue = 0; //ASSEGNO UN VALORE DI RITORNO ALLA SYSCALL
  return;
}
