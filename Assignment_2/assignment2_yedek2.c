//============================================================================//
//
//	File Name	: 504181527.c
//	Create Date	: 21.04.2019
//	Designers	: Nebi Deniz Uras
//	Number 	    : 504181527
//	Description	: Computer Operating Systems Assignment 2
//
//	Important Notes: Code was compiled and tested on ITU SSH 
//
//============================================================================//

//#define _CRT_SECURE_NO_WARNINGS

/*to be able to involve an integer variable in strcat*/
#define _GNU_SOURCE

/*for shared memory and semaphores*/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

/*for handling signals*/
#include <signal.h>

/*to use fork*/
#include <unistd.h>

/*other necessary headers*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//============================================================================//
//==========================  MACRO DEFINITIONS ==============================//
//============================================================================//
/*The ftok() function returns a key based on path and id that is usable in subsequent calls to semget() and shmget()*/
#define KEYSEM  	ftok(strcat(get_current_dir_name(), argv[0]), 1)	// for waiting of main process
#define KEYSEM1 	ftok(strcat(get_current_dir_name(), argv[0]), 2)	// for waiting of child process 1
#define KEYSEM2  	ftok(strcat(get_current_dir_name(), argv[0]), 3)	// for waiting of child process 2
#define KEYSEM3 	ftok(strcat(get_current_dir_name(), argv[0]), 4)	// for waiting of child process 3
#define KEYSHM1  	ftok(strcat(get_current_dir_name(), argv[0]), 5)	// for shared memory for numbers that can be divided by 2
#define KEYSHM2  	ftok(strcat(get_current_dir_name(), argv[0]), 6)	// for shared memory for numbers that can be divided by 3 and printing

#define DEBUG		(0)	// for debug messages

//============================================================================//
//=========================== TYPE DEFINITIONS ===============================//
//============================================================================//

//============================================================================//
//========================== FUNCTION PROTOTYPES =============================//
//============================================================================//

//============================================================================//
//============================ GLOBAL VARIABLES ==============================//
//============================================================================//

//============================================================================//
//============================ PUBLIC FUNCTIONS ==============================//
//============================================================================//

/*semaphore increment operation*/
void sem_signal(int semid, int val)
{
	struct sembuf semaphore;
	semaphore.sem_num=0;
	semaphore.sem_op=val;
	semaphore.sem_flg =1; /*relative : add sem_op to value*/
	semop(semid, &semaphore, 1);
}

/*semaphore decrement operation*/
void sem_wait(int semid, int val)
{
	struct sembuf semaphore;
	semaphore.sem_num=0;
	semaphore.sem_op=(-1 * val);
	semaphore.sem_flg =1; /*relative : add sem_op to value*/
	semop(semid, &semaphore, 1);
}

/*signal-handling function*/
void mysignal(int signum)
{
#if DEBUG
	printf("Received signal with num = %d.\n", signum);
#endif
}

void mysigset(int num)
{
	struct sigaction mysigaction;
	mysigaction.sa_handler=(void *)mysignal;
	/*using signal-catching function identified by sa_handler*/
	mysigaction.sa_flags=0;
	/*sigaction system call is used to change the action taken by a
	process on receipt of a specific signal (specified with num)*/
	sigaction(num, &mysigaction, NULL);
}

int main (int argc, char *argv[])
{
	mysigset(13); /*signal handler with num=12*/
	int shmid1 = 0; /*shared memory id 1*/
	int shmid2 = 0; /*shared memory id 2*/
	int *globalcp1 = NULL; /*shared memory area 1*/
	int *globalcp2 = NULL; /*shared memory area 2*/
	int localInt = -1; /*a locally defined integer*/
	int termSem = 0, lockP1 = 0, lockP2 = 0, lockP3 = 0; /* semaphore ids */
	int f; /*return value of fork()*/
	int child[3]; /*child process ids*/
	int currChild = -1;
	int childList[3] = {0}; /*child process list*/
	int i = 0, myOrder = 0; /*order of the running child process*/

	/*creating 3 child processes*/
	for(i=0; i<3; i++)
	{			
		f=fork();
		if(f==-1)
		{
			printf("FORK error....\n");
			exit(1);
		}

		childList[i] = getpid();

		if(f==0)
		{
			break;
		}		
		
		child[i]=f;
	}
	
	if(f != 0)
	{
		/*creating a semaphore for synchronization (value=0) between parent and its children*/
		termSem = semget(KEYSEM, 1, 0700|IPC_CREAT);
		semctl(termSem, 0, SETVAL, 0);
		
		/*creating a semaphore for synchronization (value=0) between child processes*/
		lockP1 = semget(KEYSEM1, 1, 0700|IPC_CREAT);
		lockP2 = semget(KEYSEM2, 1, 0700|IPC_CREAT);
		lockP3 = semget(KEYSEM3, 1, 0700|IPC_CREAT);
		semctl(lockP1, 0, SETVAL, 0);
		semctl(lockP2, 0, SETVAL, 0);
		semctl(lockP3, 0, SETVAL, 0);
		
		/*creating shared memory areas with the size of an int*/
		shmid1 = shmget (KEYSHM1, sizeof(int), 0700|IPC_CREAT);
		shmid2 = shmget (KEYSHM2, sizeof(int), 0700|IPC_CREAT);
		
		/*attaching the shared memory segment identified by shmid to the address space of the calling process (parent)*/
		globalcp1 = (int*)shmat(shmid1, 0, 0);
		globalcp2 = (int*)shmat(shmid2, 0, 0);
		
		/*initializing the value in the shared memory as -1*/
		*globalcp1 = -1;
		*globalcp2 = -1;
		
		/*detaching the shared memory segment from the address space of the calling process (parent)*/
		shmdt(globalcp1);
		shmdt(globalcp2);
		
		/*waiting for 2 seconds*/	
		sleep(2); 
		
		printf("Parent has created resources.\n");
		printf("Now, it will start the child processes.\n");
		
		/*sending the signal 12 to start child processes*/
		for(i=0;i<3;i++)
		{
			kill(child[i],13);
		}
		
		/*decreasing semaphore value by 3(wait for all children)*/
		printf("Parent waits.\n");
		sem_wait(termSem, 3);
		printf("Parent continues.\n");


		/* removing the created semaphores and shared memories */
		semctl(termSem, 0, IPC_RMID, 0); 
		semctl(lockP1, 0, IPC_RMID, 0);
		semctl(lockP2, 0, IPC_RMID, 0);
		semctl(lockP3, 0, IPC_RMID, 0);		
		shmctl(shmid1, IPC_RMID, 0);
		shmctl(shmid2, IPC_RMID, 0);
		
		/*parent process is exiting*/
		exit(0);
	}
	else
	{
		/* to show which child process is running */
		myOrder = i;

		/* wait until receiving a signal (kill signal)*/
		pause();
		printf("child %d is starting....\n", myOrder + 1);

		/*returning the semaphore ids for KEYSEM,KEYSEM1,KEYSEM2,KEYSEM3 */
		lockP1 = semget(KEYSEM1, 1, 0);
		lockP2 = semget(KEYSEM2, 1, 0);
		lockP3 = semget(KEYSEM3, 1, 0);
		termSem = semget(KEYSEM, 1, 0);

		/* returning the shared memory id associated with KEYSHM1 and  KEYSHM2*/
		shmid1 = shmget(KEYSHM1, sizeof(int), 0);
		shmid2 = shmget(KEYSHM2, sizeof(int), 0);

		/* attaching the shared memory segment identified by shmid to the address space of the calling process (child) */
		globalcp1 = (int*)shmat(shmid1, 0, 0);
		globalcp2 = (int*)shmat(shmid2, 0, 0);

		//printf("currChild = %d\tgetpid = %d\n", currChild, getpid());
		if (childList[0] == getpid())
		{
			for (i = 1; i <= 1001; i++)
			{
				//printf("Child 1 processing %d i = %d\n", getpid(), i);

				//printf("child %d : Found the value : %d, i :%d\n", myOrder, *globalcp1, i);

				/* updating the value of the shared memory segment */
				if (i == 1001)
				{
					*globalcp1 = 0;
				}
				else if (i % 2 != 0)
				{
					*globalcp1 = i;
				}
				else // if it is not divided by 2, continue next number
				{
					continue;
				}

				///* updating the value of the shared memory segment */
				//localInt = *globalcp1;
				//sleep(1); /* waiting for a second */
				//localInt = i;
				//*globalcp1 = localInt;
				//printf("child %d : Made the value: %d, i :%d\n", myOrder, *globalcp1, i);

				/* making the critical section available again for child process 2*/
				sem_signal(lockP2, 1);
				//sleep(1); /* waiting for a second */
				//printf("Child 1 waits %d\n", getpid());
				/* waiting for the semaphore with id=lockP1 to enter the critical section */
				sem_wait(lockP1, 1);
			}

			/* increase semaphore by 1(synchronization with parent)*/
			sem_signal(termSem, 1);
			sleep(1); /* waiting for a second */
		}
		else if (childList[1] == getpid())
		{
			//printf("currChild2 = %d\tgetpid = %d\n", childList[1], getpid());

			while (1)
			{
				/* waiting for the semaphore with id=lockP2 to enter the critical section */
				//printf("Child 2 waits %d\n", getpid());

				sem_wait(lockP2, 1);
				//printf("Child 2 processing %d\n", getpid());

				localInt = *globalcp1;
				//printf("localInt %d\n", localInt);
				if (localInt == 0)
				{
					//printf("Child 2 processing2 %d\n", getpid());

					*globalcp2 = 0;
					/* making the critical section available again for child process 1 beacuse of number cant be divided by 3*/
					sem_signal(lockP3, 1);
					sleep(1); /* waiting for a second */

					break;
				}

				/* updating the value of the shared memory segment */
				if (localInt % 3 == 0)
				{
					//printf("localInt %d\n", localInt);

					*globalcp2 = localInt;

					/* making the critical section available again for child process 3 beacuse of number can be divided by 3 */
					sem_signal(lockP3, 1);
					//sleep(1); /* waiting for a second */
				}
				else
				{
					/* making the critical section available again for child process 1 beacuse of number cant be divided by 3*/
					sem_signal(lockP1, 1);
					//sleep(1); /* waiting for a second */
				}				
			}

			/* increase semaphore by 1(synchronization with parent)*/
			sem_signal(termSem, 1);
			sleep(1); /* waiting for a second */
		}
		else if (childList[2] == getpid())
		{
			while (1)
			{
				/* waiting for the semaphore with id=lock to enter the critical section */
				sem_wait(lockP3, 1);
				//printf("Child 3 processing %d\n", getpid());

				localInt = *globalcp2;
				if (localInt == 0)
				{
					break;
				}
				/* updating the value of the shared memory segment */
				//fflush(stdout);
				printf("%d \n", localInt);

				/* making the critical section available again for child process 1 */
				sem_signal(lockP1, 1);
				sleep(1); /* waiting for a second */
			}

			/* increase semaphore by 1(synchronization with parent)*/
			sem_signal(termSem, 1);
			sleep(1); /* waiting for a second */
		}

		/* detaching the shared memory segment from the address	space of the calling process (child) */
		shmdt(globalcp1);
		shmdt(globalcp2);

		/* child process is exiting */
		exit(0);
	}

	if (f != 0)
	//printf("Parent has deleted resources.\n");
	
	return 0;
}
	
	
	
	