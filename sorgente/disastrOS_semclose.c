#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  int sd= running->syscall_args[0]; 
  
  printf("Chiudo il semaforo con fd: %d...\n",sd);
  SemDescriptor* sfd = SemDescriptorList_byFd(&running->sem_descriptors,sd); //RICERCO IL DESCRITTORE DEL SEMAFORO DALLA LISTA DEL PROCESSO
  
  //CONTROLLO SE IL FD E' PRESENTE, ALTRIMENTI TERMINO
  if(!sfd){
	  printf("ERRORE: fd: %d non trovato\n", sd);
	  return;
  }
 
  sfd = (SemDescriptor*)List_detach(&running->sem_descriptors,(ListItem*)sfd); //LO RIMUOVO DALLA LISTA DEI DESCRITTORI DEL PROCESSO
  if(!sfd){
	  printf("ERRORE: rimozione del descrittore: %d non riuscita\n",sfd->fd);
  }
  
  Semaphore* sem = sfd->semaphore;  //RICHIAMO IL SEMAFORO ASSOCIATO AL DESCRITTORE
 
  
  SemDescriptorPtr* sdptr = (SemDescriptorPtr*)List_detach(&sem->descriptors,(ListItem*)sfd->ptr); //RIMUOVO IL PUNTATORE AL DESCRITTORE DALLA LISTA INTERNA AL SEMAFORO 
  
  //CONTROLLO CHE L'OPERAZIONE SIA ANDATA A BUON FINE
  if(!sdptr){
	  printf("ERRORE: rimozione del puntatore al descrittore %d non riuscita\n",sdptr->descriptor->fd);
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
  
  //QUANDO IL SEMAFORO NON HA PIU' DESCRITTORI ATTIVI LO RIMUOVO DALLA LISTA DEL PROCESSO E LO DEALLOCO
  if(sem->descriptors.size == 0 && sem->waiting_descriptors.size == 0){
	  Semaphore* aux = (Semaphore*)List_detach(&semaphores_list,(ListItem*)sem);
	  if(!aux){
		  printf("ERRORE: Rimozione del semaforo: %d non riuscita\n",aux->id);
		  return;
	  }
	  ret = Semaphore_free(sem);
	  if(ret){
		  printf("ERRORE: Free del semaforo %d non riuscita\n",sem->id);
		  return;
	  }
  }
  
  
  
  running -> syscall_retvalue = 0;
  
  printf("Fatto!\n");
  
  
  
}
