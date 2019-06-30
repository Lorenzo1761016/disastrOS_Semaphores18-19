#include <stdio.h>
#include <unistd.h>
#include "disastrOS.h"
#include "disastrOS.c"

#define BUFFER_LENGTH 50
#define ITERATION 10

int buffer[BUFFER_LENGTH];
int write_index = 0;
int read_index = 0;
int var = 1;

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);
  printf("PID: %d, terminating\n", disastrOS_getpid());
    
  printf("APERTURA SEMAFORI...\n");
   //TEST DI APERTURA SEMAFORI
  int empty_sem = disastrOS_semOpen(1,0);
  int full_sem = disastrOS_semOpen(2,BUFFER_LENGTH);
  int read_sem = disastrOS_semOpen(3,1);
  int write_sem = disastrOS_semOpen(4,1);

  for (int i=0; i<ITERATION; ++i){  
	
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);  
    
    //SE IL PID DEL FIGLIO E' UN NUMERO PARI ESEGUO UNA SCRITTURA ALL'INTERNO DEL BUFFER
    if(disastrOS_getpid()%2 == 0){
		disastrOS_semWait(full_sem);
		disastrOS_semWait(write_sem);
		
		//SEZIONE CRITICA DI SCRITTURA	
		printf("[WRITE] SCRIVO NELLA CELLA %d DEL BUFFER IL VALORE: %d\n",write_index, var);
		buffer[write_index] = var;
		write_index = (write_index+1)%BUFFER_LENGTH; //UTILIZZO UN BUFFER CIRCOLARE
		var++;
		
		disastrOS_sleep(20);
		
		disastrOS_semPost(write_sem);
		disastrOS_semPost(empty_sem);
	}
	//SE IL PID DEL FIGLIO E' INVECE DISPARI ESEGUO UNA LETTURA ALL'INTERNO DEL BUFFER
	else{ 
		disastrOS_semWait(empty_sem);
		disastrOS_semWait(read_sem);
		
		
		//SEZIONE CRITICA DI LETTURA
		int val = buffer[read_index];
		read_index = (read_index+1)%BUFFER_LENGTH; // UTILIZZO UN BUFFER CIRCOLARE
		
		disastrOS_sleep(20);
		
		printf("[READ] VALORE LETTO NELLA CELLA %d DEL BUFFER: %d\n", read_index,val);
		
		disastrOS_semPost(read_sem);
		disastrOS_semPost(full_sem);
		}
		
		printf("BUFFER\n");
		int i;
		for(i = 0;i<BUFFER_LENGTH;i++){
			printf("%d ",buffer[i]);
		}
		printf("\n");
	}
		
		
  
  printf("CHIUSURA SEMAFORI...\n");
  //TEST DI CHIUSURA SEMAFORI
  disastrOS_semClose(empty_sem); 
  disastrOS_semClose(full_sem); 
  disastrOS_semClose(read_sem); 
  disastrOS_semClose(write_sem); 
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  int fd[10];
  

  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    fd[i]=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd[i]);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }
  
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  
  printf("DEALLOCO LE RISORSE...\n");
  //CHIUDO E DEALLOCO LE RISORSE QUANDO TUTTI I PROCESSI HANNO SVOLTO LA LORO ATTIVITA'
  int i;
  for (i = 0; i <ITERATION; ++i)
  {
	printf("Dealloco la risorsa %d...\n",fd[i]);
	disastrOS_closeResource(fd[i]);
	disastrOS_destroyResource(i);
	disastrOS_printStatus();
  }
  
  
  printf("STATO FINALE DEL SISTEMA:\n");
  disastrOS_printStatus();
  
  printf("BUFFER DOPO LE OPERAZIONI DI SCRITTURA...\n");
  for(i=0;i<BUFFER_LENGTH;i++){
	  printf("%d ", buffer[i]);
  }
  printf("\n");
  printf("shutdown!\n");
  disastrOS_shutdown();
}


int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
