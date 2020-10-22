/* Wrapper class for semaphore set */
#pragma once

#include <sys/types.h>

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};


class Semaphore {
	private:
		int m_id;
		int m_num_sems;
	public:
		Semaphore(int m_num_sems, int val);
		~Semaphore();
		
		/* Increase sem_num's  sem_val by 1 */
		void up(int sem_num);
		
		/* Increase sem_num's  sem_val by val */
		void up(int sem_num, int val);

		/* Decrease sem_num's  sem_val by 1 */
		void down(int sem_num);
		
		/* Decrease sem_num's  sem_val by val */
		void down(int sem_num, int val);

		/* Block the calling process until sem_num's sem_val becomes 0 */
		void waitforzero(int sem_num);

		/* Destroy the semaphore set */
		void clean();
};