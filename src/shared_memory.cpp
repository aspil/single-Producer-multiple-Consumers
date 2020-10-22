#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "../include/shared_memory.h"

Shared_Memory::Shared_Memory(size_t mem_size) {
	key_t shm_key;
	this->m_size = mem_size;
	/* Generate a unique key for shmget */
	if ((shm_key = ftok("src/feeder.cpp", 'R')) == -1) {
		perror("Error in ftok() called by Shared_Memory constructor: ");
		exit(EXIT_FAILURE);
	}

	/* Get an id for the shared memory segment */
	if ((this->m_id = shmget(shm_key, mem_size, IPC_CREAT | 0666)) < 0) {
		perror("Error in shmget() called by Shared_Memory constructor: ");
		exit(EXIT_FAILURE);
	}
}

Shared_Memory::~Shared_Memory() {
	
}

void* Shared_Memory::attach() {
	this->m_data = shmat(this->m_id, (void *)0, 0);
	if (this->m_data == (void*) -1) {
		perror("Error in shmat() called by Shared_Memory::attach() : ");
		exit(EXIT_FAILURE);
	}
	return this->m_data;
}

void Shared_Memory::detach() {
	if (shmdt((void*) this->m_data) == -1) {
		perror("Error in shmdt() called by Shared_Memory::detach() : ");
		exit(EXIT_FAILURE);
	}
}

void Shared_Memory::clean() {
	if (shmctl(this->m_id, IPC_RMID, NULL) == -1) {
		perror("Error in shmctl() called by Shared_Memory::clean() : ");
		exit(EXIT_FAILURE);
	}
}