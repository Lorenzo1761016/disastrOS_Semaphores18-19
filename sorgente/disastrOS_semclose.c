#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  int semnum = running->syscall_args[0]; 
  
  printf("Chiudo il semaforo %d...",semnum);
  SemDescriptor* sfd = SemDescriptorList_byFd(&running->sem_descriptors,semnum); //RICERCO IL DESCRITTORE DEL SEMAFORO DALLA LISTA DEL PROCESSO
 
  List_detach(&running->sem_descriptors,sfd); //LO RIMUOVO DALLA LISTA
  
  Semaphore* sem = sfd->semaphore;
  
  List_detach(&sem->descriptors,sfd->ptr); //RIMUOVO IL PUNTATORE AL DESCRITTORE DALLA LISTA INTERNA AL SEMAFORO 
  
  printf("Fatto!\n");
  
  
  
  
}
