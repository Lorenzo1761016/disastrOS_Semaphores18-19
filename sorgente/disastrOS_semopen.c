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
  
  
  //VERIFICO CHE L'ID DEL SEMAFORO NON SIA NEGATIVO
  if (semnum < 0){
	  printf("Errore id semaforo");
	  return;
  }
  
  printf("Creo il semaforo %d...\n", semnum);
  
  Semaphore* aux = SemaphoreList_byId(&semaphores_list,semnum); //VERIFICO SE IL SEMAFORO (SEMNUM) E' GIA' PRESENTE NELLA LISTA DEI SEMAFORI
  
  if (aux == NULL) {
	  aux = Semaphore_alloc(semnum,counter);  //SE IL SEMAFORO (SEMNUM) NON ESISTE LO ALLOCO
	  if(!aux){
		  printf("ERRORE: Allocazione semaforo fallita\n");
		  return;
	  }
	  List_insert(&semaphores_list,semaphores_list.last,(ListItem*)aux); //INSERISCO IL SEMAFORO (SEMNUM) ALLA LISTA DEI SEMAFORI
	  }

  SemDescriptor* sfd = SemDescriptor_alloc(running -> last_sem_fd, aux, running); //IN OGNI CASO ALLOCO IL DESCRIPTOR DEL SEMAFORO (SEMNUM)
	
  //CONTROLLO CHE L'ALLOCAZIONE SIA RIUSCITA	
  if(!sfd){
	  printf("ERRORE: Allocazione descriptor del semaforo fallita\n");
	  return;
  }

  SemDescriptorPtr* sfdptr = SemDescriptorPtr_alloc(sfd); //ALLOCO IL PUNTATORE AL DESCRITTORE (SFD) DEL SEMAFORO (SEMNUM) 
  
  //CONTROLLO CHE L'ALLOCAZIONE SIA RIUSCITA
  if(!sfdptr) {
	  printf("ERRORE: Allocazione puntatore al descrittore del semaforo fallita\n");
	  return;
  }
  
  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*)sfd); //AGGIUNGO IL DESCRITTORE SFD ALLA LISTA DEI DESCRITTORI DEI SEMAFORI
  
  (running->last_sem_fd)++; //INCREMENTO IL CONTATORE INTERNO AL PCB DEI SEMAFORI APERTI
  
  sfd->ptr = sfdptr; //INSERISCO IL PUNTATORE AL DESCRITTORE ALL'INTERNO DELLA STRUCT DELL DESCRITTORE
  
  List_insert(&aux->descriptors,aux->descriptors.last,(ListItem*)(sfd->ptr));  //AGGIUNGO IL PUNTATORE AL DESCRITTORE DEL SEMAFORO NELLA LISTA DEI DESCRITTORI DEL SEMAFORO CORRENTE
  
  running->syscall_retvalue = sfd->fd; //ASSEGNO COME VALORE DI RITORNO DELLA SYSCALL IL FILE DESCRIPTOR DEL SEMAFORO 
	
  printf("Fatto!\n");	  
}

