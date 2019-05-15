#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  int semnum = running -> syscall_args[0]; //IL PRIMO ARGOMENTO DELLA SYSCALL E' L'ID DEL SEMAFORO
  int counter = running -> syscall_args[1]; //IL SECONDO ARGOMENTO DELLA SYSCALL E' IL COUNTER DEL SEMAFORO (SEMNUM)
  
  printf("Creo il semaforo %d...", semnum);
  
  Semaphore* aux = SemaphoreList_byId(&semaphores_list,semnum); //VERIFICO SE IL SEMAFORO (SEMNUM) E' GIA' PRESENTE NELLA LISTA DEI SEMAFORI
  
  if (aux = NULL) {
	  Semaphore* sem = Semaphore_alloc(semnum,counter);  //SE IL SEMAFORO (SEMNUM) NON ESISTE LO ALLOCO
	  List_insert(&semaphores_list,semaphores_list.last,sem); //INSERISCO IL SEMAFORO (SEMNUM) ALLA LISTA DEI SEMAFORI
	  }
  SemDescriptor* sfd = SemDescriptor_alloc(running -> last_sem_fd, sem, running); //IN OGNI CASO ALLOCO IL DESCRIPTOR DEL SEMAFORO (SEMNUM)
	  
	 
	  
  SemDescriptorPtr* sfdptr = SemDescriptorPtr_alloc(sfd); //ALLOCO IL PUNTATORE AL DESCRITTORE (SFD) DEL SEMAFORO (SEMNUM) 
  
  List_insert(running->sem_descriptors, sem_descriptors.last, sfdptr); //AGGIUNGO IL PUNTATORE SFDPTR ALLA LISTA DEI PUNTATORI AI DESCRITTORI DEI SEMAFORI
	
  printf("Fatto!\n");	  
}

