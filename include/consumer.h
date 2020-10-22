#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include "shared_memory.h"
#include "semaphores.h"

using namespace std;

class Consumer {
	private:
		int *m_numbers;
		int m_size;
		int m_curr_pos;
		long double m_latency;
		long double m_average_latency;

	public:
		Consumer(size_t array_size);
		~Consumer();
		
		/* Calculate the time it took for the consumer to read a number from the shared memory since the number's creation */
		void calc_latency(const struct timespec& curr_time, const struct timespec& elem_added_time);

		/* Calculate the average of latency of the consumer */
		long double get_average_latency();

		/* Read a number from the shared memory and calculate the latency */
		void consume(const struct shm_ent& ent, const struct timespec& curr_time);

		/* Read numbers from the shared memory until m_numbers array is full */
		void start_consuming(Semaphore& Empty, Semaphore& Full, struct shm_ent* shared_data);

		/* Save the id of the process owning a Consumer object, the average latency and the array in a log file */
		void log_results(Semaphore& Mutex, std::string args);
		
		/* Save the id of the process owning a Consumer object, the average latency in a log file */
		void log_times(Semaphore& Mutex, std::string args);
};
