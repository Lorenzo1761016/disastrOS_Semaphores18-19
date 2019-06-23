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
  
  //MI ASSICURO CHE IL CONTATORE SIA MAGGIORE DI 0, ALTRIMENTI NON POSSO DECREMENTARLO
  if((sfd->semaphore)->count > 0){
	  (sfd->semaphore)->count--;
  }
  else{
	  printf("Wait fallita\n");
	  running->syscall_retvalue = -1;
	  return;
  }
  
  running->syscall_retvalue = 0; //ASSEGNO UN VALORE DI RITORNO ALLA SYSCALL
  return;
}
