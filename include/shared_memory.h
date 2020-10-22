/* Wrapper class for semaphore set */
#pragma once

#include <ctime>

typedef struct shm_ent {
	int elem;
	struct timespec timestamp;
} shm_ent;


class Shared_Memory {
	private:
		int m_id;
		size_t m_size;
		void* m_data;
		
	public:
		Shared_Memory(size_t mem_size);
		~Shared_Memory();

		/* Attach a shared memory segment and return a pointer to it */
		void* attach();

		/* Detach a shared memory segment */
		void detach();

		/* Clear the shared memory segment */
		void clean();

};
