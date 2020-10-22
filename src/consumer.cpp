#include <iostream>
#include <cstring>
#include <string>
#include <ctime>
#include <fstream>
#include <unistd.h>

#include "../include/shared_memory.h"
#include "../include/semaphores.h"
#include "../include/consumer.h"

Consumer::Consumer(size_t array_size) : m_size(array_size) {
	m_curr_pos = 0;
	m_latency = m_average_latency = 0.0;
	this->m_numbers = new int[array_size];
	memset(this->m_numbers, 0, array_size * sizeof(int));
}
Consumer::~Consumer() {
	delete [] m_numbers;
}
void Consumer::calc_latency(const struct timespec& curr_time, const struct timespec& elem_added_time) {
	long sec = curr_time.tv_sec - elem_added_time.tv_sec;
	long nsec = curr_time.tv_nsec - elem_added_time.tv_nsec;

	this->m_latency += (long double) (sec + (long double) nsec / 10e9);
}

long double Consumer::get_average_latency() {
	if (this->m_average_latency == 0)
		this->m_average_latency = (long double) this->m_latency / m_size;
	return m_average_latency;
}

void Consumer::consume(const struct shm_ent& ent, const struct timespec& curr_time) {
	this->m_numbers[m_curr_pos] = ent.elem;
	calc_latency(curr_time, ent.timestamp);
}

void Consumer::start_consuming(Semaphore& Empty, Semaphore& Full, struct shm_ent* shared_data) {
	struct timespec curr_time;

	while (m_curr_pos != m_size) {

		if (m_numbers + m_curr_pos * sizeof(int) == NULL)	// Stop looping if we are out of the array's limits. Means we're done
			break;
		Full.down(0); 	// Gain access to the shared memory resource

		clock_gettime(CLOCK_MONOTONIC, &curr_time);		// Time the request was approved
		consume(*shared_data, curr_time);

		/* Block until all consumers read from the shared memory */
		Full.waitforzero(0);
		/* Unblock the feeder */
		Empty.down(0);
		m_curr_pos++;
	}
}

void Consumer::log_results(Semaphore& Mutex, std::string args) {
	std::ofstream outfile;
	std::string filename = "results_" + args + ".log";
	
	Mutex.down(0);	// Only the current consumer can write its results to the file
	outfile.open(filename.c_str(), std::ios_base::app); // Append to the file
	outfile << "Process ID: " << getpid() << " - Average latency: " << get_average_latency() << "\n";
	/* Print the integer array with 40 numbers in each line */
	for (int i = 0; i < this->m_size; ++i) {
		outfile << this->m_numbers[i] << " ";

		if ((i+1) % 40 == 0)	// For formatting purposes
			outfile << "\n";
	}
	outfile << "\n\n";

	Mutex.up(0);	// Let another consumer do the same job
}

void Consumer::log_times(Semaphore& Mutex, std::string args) {
	std::ofstream outfile;
	std::string filename = "average_times_" + args + ".log";

	Mutex.down(0);
	outfile.open(filename.c_str(), std::ios_base::app);
	outfile << "Process ID: " << getpid() << " - Average latency: " << get_average_latency() << "\n";

	Mutex.up(0);
}