#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  int sd = running->syscall_args[0];
  
  printf("POST...\n");
  SemDescriptor* sfd = SemDescriptorList_byFd(&running->sem_descriptors,sd); //RICERCO IL DESCRITTORE DEL SEMAFORO NELLA LISTA DEL PCB
  
  //VERIFICO CHE SIA PRESENTE, ALTRIMENTI TERMINO
  if(!sfd){
	  printf("ERRORE: Il semaforo %d non esiste!\n",sd);
	  return;
  }
  
  (sfd->semaphore)->count++;  //INCREMENTO IL SUO CONTATORE
  
  //SETTO IL VALORE DI RITORNO DELLA SYSCALL
  running->syscall_retvalue = 0;
  return;
  
}
