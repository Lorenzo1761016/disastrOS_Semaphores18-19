#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  int semnum = running->syscall_args[0]; 
  
  printf("Chiudo il semaforo %d...\n",semnum);
  SemDescriptor* sfd = SemDescriptorList_byFd(&running->sem_descriptors,semnum); //RICERCO IL DESCRITTORE DEL SEMAFORO DALLA LISTA DEL PROCESSO
  
  //CONTROLLO SE IL SEMAFORO CHE VOGLIO CHIUDERE E' PRESENTE NELLA LISTA DEI SEMAFORI ATTIVI
  if(!sfd){
	  printf("Errore. Semaforo semnum: %d non trovato\n", semnum);
	  return;
  }
 
  List_detach(&running->sem_descriptors,(ListItem*)sfd); //LO RIMUOVO DALLA LISTA
  
  Semaphore* sem = sfd->semaphore;
  
  SemDescriptorPtr* sdptr = (SemDescriptorPtr*)List_detach(&sem->descriptors,(ListItem*)sfd->ptr); //RIMUOVO IL PUNTATORE AL DESCRITTORE DALLA LISTA INTERNA AL SEMAFORO 
  
  if(!sdptr){
	  printf("Errore nella rimozione del puntatore al descrittore\n");
	  return;
  }
  
  printf("Fatto!\n");
  
  
  
  
}
