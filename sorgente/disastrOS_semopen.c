#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_globals.h"

void internal_semOpen(){
  int semnum = running -> syscall_args[0]; //IL PRIMO ARGOMENTO DELLA SYSCALL E' L'ID DEL SEMAFORO
  int counter = running -> syscall_args[1]; //IL SECONDO ARGOMENTO DELLA SYSCALL E' IL COUNTER DEL SEMAFORO (SEMNUM)
  
  
  
  if (semnum < 0){
	  printf("Errore id semaforo");
	  return;
  }
  
  printf("Creo il semaforo %d...", semnum);
  
  Semaphore* aux = SemaphoreList_byId(&semaphores_list,semnum); //VERIFICO SE IL SEMAFORO (SEMNUM) E' GIA' PRESENTE NELLA LISTA DEI SEMAFORI
  
  if (aux == NULL) {
	  aux = Semaphore_alloc(semnum,counter);  //SE IL SEMAFORO (SEMNUM) NON ESISTE LO ALLOCO
	  List_insert(&semaphores_list,semaphores_list.last,(ListItem*)aux); //INSERISCO IL SEMAFORO (SEMNUM) ALLA LISTA DEI SEMAFORI
	  }

  SemDescriptor* sfd = SemDescriptor_alloc(running -> last_sem_fd, aux, running); //IN OGNI CASO ALLOCO IL DESCRIPTOR DEL SEMAFORO (SEMNUM)
	  
	 
	  
  SemDescriptorPtr* sfdptr = SemDescriptorPtr_alloc(sfd); //ALLOCO IL PUNTATORE AL DESCRITTORE (SFD) DEL SEMAFORO (SEMNUM) 
  
  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*)sfdptr); //AGGIUNGO IL PUNTATORE SFDPTR ALLA LISTA DEI PUNTATORI AI DESCRITTORI DEI SEMAFORI
  
  running->last_sem_fd++; //INCREMENTO IL CONTATORE INTERNO AL PCB DEI SEMAFORI APERTI
  
  sfd->ptr = sfdptr; //INSERISCO IL PUNTATORE AL DESCRITTORE ALL'INTERNO DELLA STRUCT DELL DESCRITTORE
  
  running->syscall_retvalue = sfd->fd; //ASSEGNO COME VALORE DI RITORNO DELLA SYSCALL IL FILE DESCRIPTOR DEL SEMAFORO 
	
  printf("Fatto!\n");	  
}

