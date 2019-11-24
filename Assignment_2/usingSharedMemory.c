#include <stdio.h>
#include <sys/shm.h> /* shared memory */
#include <sys/stat.h> /* S_IRUSR and S_IWUSR */

int main()
{
	/* Allocate a Shared Memory Location */
	int segmentnt_id;
	const int shared_segment_size = 0x6400;
	segment_id = shmget(IPC_PRIVATE, shared_segment_size, IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
	 /* IPC_PRIVATE : shared memory cannot be accessed by other processes
		IPC_CREAT : create the segment if it doesn't already exist
		IPC_EXCL : fail if segment already exists
		S_IRUSR : read permission, owner
		S_IWUSR : write permission, owner */
	
	/* At tach a Conne c t ion */
	char * shared_memory;
	/* If shmaddr is NULL, the system chooses a suitable(unused) address at which to attach the segment. */
	shared_memory = (char *)shmat(segment_id, 0, 0);
	printf("Shared Memory Attached Address %p\n", shared_memory);

	/* Learn the Segment Size */
	int segment_size;
	struct shmid_ds shmbuffer;
	shmctl(segment_id, IPC_STAT, &shmbuffer);
	segment_size = shmbuffer.shmsegsz;
	printf("Segment size is: %d\n", segment_size);
	
	/* Write a String into the Shared Memory Location */
	sprintf(shared_memory, "Hello, World.");
	
	/* Detach Connection */	
	/* Make a Shared Memory Connection to another Address */
	shared_memory = (char *)shmat(segment_id, (void *) 0x5000000, 0);
	printf("Shared Memory New Connection Address %p\n", shared_memory);

	/* Read String from the Shared Memory Location */
	printf("%s\n ", shared_memory);

	/* Detach Connection */
	shmdt(shared_memory);

	/* Remove the Shared Memory Segment */
	shmctl(segment_id, IPC_RMID, 0);

	return 0;
}