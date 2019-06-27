#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  int sd= running->syscall_args[0]; 
  
  printf("Chiudo il semaforo...\n");
  SemDescriptor* sfd = SemDescriptorList_byFd(&running->sem_descriptors,sd); //RICERCO IL DESCRITTORE DEL SEMAFORO DALLA LISTA DEL PROCESSO
  
  //CONTROLLO SE IL SEMAFORO CHE VOGLIO CHIUDERE E' PRESENTE NELLA LISTA DEI SEMAFORI ATTIVI
  if(!sfd){
	  printf("Errore. fd: %d non trovato\n", sd);
	  return;
  }
 
  sfd = (SemDescriptor*)List_detach(&running->sem_descriptors,(ListItem*)sfd); //LO RIMUOVO DALLA LISTA DEI DESCRITTORI DEL PROCESSO
  
  Semaphore* sem = sfd->semaphore;  //RICHIAMO IL SEMAFORO ASSOCIATO AL DESCRITTORE
 
  
  SemDescriptorPtr* sdptr = (SemDescriptorPtr*)List_detach(&sem->descriptors,(ListItem*)sfd->ptr); //RIMUOVO IL PUNTATORE AL DESCRITTORE DALLA LISTA INTERNA AL SEMAFORO 
  
  //CONTROLLO CHE L'OPERAZIONE SIA ANDATA A BUON FINE
  if(!sdptr){
	  printf("ERRORE: rimozione del puntatore al descrittore non riuscita\n");
	  return;
  }
  
  
  
  
  
  //LIBERO DALLA MEMORIA IL PUNTATORE AL DESCRITTORE DEL SEMAFORO
  int ret = SemDescriptorPtr_free(sdptr);
  if(ret){
	  printf("ERRORE: Free del puntatore al fd %d non riuscita\n",sdptr->descriptor->fd);
	  return;
  }
  
  //LIBERO DALLA MEMORIA IL DESCRITTORE DEL SEMAFORO
  ret = SemDescriptor_free(sfd);
  if(ret){
	  printf("ERRORE: Free del fd %d non riuscita\n",sfd->fd);
	  return;
  }
  
  /*ret = Semaphore_free(sem);
  if(ret){
	  printf("ERRORE: Free del semaforo %d non riuscita\n",sem->id);
	  return;
  }*/
  
  
  running -> syscall_retvalue = 0;
  
  printf("Fatto!\n");
  
  
  
  
}
