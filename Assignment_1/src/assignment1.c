//============================================================================//
//
//	File Name	: 504181527.c
//	Create Date	: 10.03.2019
//	Designers	: Nebi Deniz Uras
//	Number 	    : 504181527
//	Description	: Computer Operating Systems Assignment 1
//
//	Important Notes: Code was compiled and tested on ITU SSH 
//
//============================================================================//

#define _CRT_SECURE_NO_WARNINGS

//============================================================================//
//=============================== INCLUDES ===================================//
//============================================================================//
#include <stdio.h> 		// printf
#include <unistd.h> 	// fork
#include <stdlib.h> 	// exit
#include <sys/wait.h> 	// wait

//============================================================================//
//==========================  MACRO DEFINITIONS ==============================//
//============================================================================//

//============================================================================//
//=========================== TYPE DEFINITIONS ===============================//
//============================================================================//
typedef struct _ProcessNode
{
	int left;	// left child process
	int right;	// right child process
	int pid;	// process id
	int level;	// level of process in process tree
}ProcessNode;

//============================================================================//
//========================== FUNCTION PROTOTYPES =============================//
//============================================================================//

//============================================================================//
//============================ GLOBAL VARIABLES ==============================//
//============================================================================//

//============================================================================//
//============================ PUBLIC FUNCTIONS ==============================//
//============================================================================//
int main(void)
{
	ProcessNode root;
	
	// set details of first process
	root.left  = 0;
	root.right = 0;
	root.level = 1;
	root.pid   = getpid();

	root.left = fork();	// start fork from left side
	if (root.left == -1)
	{
		printf("Error\n");
		exit(1);
	}
	else if (root.left == 0)
	{
		root.level++;			// increased level for 2
		root.pid = getpid();	// get left child pid
		
		root.left = fork();		// second fork for level 3 left
		if (root.left == 0)
		{
			root.level++;			// increased level for 3
			root.pid = getpid();	// get left-left child pid
		}
	}
	else // continue to fork from right side in parent process flow
	{		
		root.right = fork();	// first fork for level 2 right
		if (root.right == 0)
		{
			root.level++;			// increased level for 2
			root.pid = getpid();	// get right child pid
									
			root.left = fork();		// second fork for left of right child in level 3
			if (root.left == 0)
			{
				root.level++;			// increased level for 3
				root.pid = getpid();	// get right-left child pid
			}
			else
			{
				root.right = fork();	// second fork for right of right child in level 3
				if (root.right == 0)
				{
					root.level++;			// increased level for 3
					root.pid = getpid();	// get right-right child pid

					root.left = fork();		// third fork for left of right-right child in level 4
					if (root.left == 0)
					{
						root.level++;			// increased level for 4
						root.pid = getpid();	// get right-right-left child pid
					}
					else
					{
						root.right = fork();	// third fork for right of right-right child in level 4
						if (root.right == 0)
						{
							root.left = 0;			// set left child as 0 for right child in level 4
							root.level++;			// increased level for 4
							root.pid = getpid();	// get right-right-right child pid
						}
					}
				}
			}
		}	
	}

	// Wait for every child
	wait(NULL);
	wait(NULL);
	wait(NULL);
	wait(NULL);
	wait(NULL);
	wait(NULL);
	wait(NULL);

	// show process details
	printf("(Seviye : %d) - (PID : %08d) - (Sol Cocuk PID : %08d) - (Sag Cocuk PID : %08d)\n", root.level, root.pid, root.left, root.right);

#if _WIN32
	system("pause");
#endif		

	return 0;
}
