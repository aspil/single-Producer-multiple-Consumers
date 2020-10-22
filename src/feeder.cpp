#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <errno.h>

#include <ctime>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/ipc.h>
#include <sys/types.h>

#include "../include/consumer.h"
#include "../include/semaphores.h"
#include "../include/shared_memory.h"

using std::cout;
using std::endl;

int validate_args(int argc, char** argv);

int main(int argc, char **argv) {
	int array_size, n_processes;
	int status = 0;
	int w_pid;
	pid_t pid;


	srand(time(NULL));

	/* Check the correctness of the program's arguments */
	if (validate_args(argc, argv) == 1) {
		n_processes = atoi(argv[2]);
		array_size = atoi(argv[4]);
		if (n_processes <= 0 || array_size < 3000) {
			cout << "The number of consumers must be positive and the size of the array > 3000" << endl;
			exit(EXIT_FAILURE);
		}
	}
	else {
		exit(EXIT_FAILURE);
	}
	std::string args = std::to_string(n_processes) + "_" + std::to_string(array_size);

	/* Initialize the array of random integers that
	 * will be copied to the child processes via the
	 * shared memory */
	int *M = new int[array_size];
	for (int i = 0; i < array_size; ++i)
		M[i] = rand() % 30;
	
	/* Initialize three sets of one semaphore each */
	Semaphore Empty(1,0);
	Semaphore Full(1,0);
	Semaphore Mtx(1,1);

	/* Initialize the required shared memory segment */
	Shared_Memory Shm(sizeof(struct shm_ent));
	struct shm_ent* shared_data = (struct shm_ent*) Shm.attach();

	/* Start the C processes */
	for (int i = 0; i < n_processes; ++i) {
		pid = fork();

		if (pid == 0) {
			Consumer C(array_size);
			C.start_consuming(Empty, Full, shared_data);
			C.log_results(Mtx, args);
			C.log_times(Mtx, args);
			return EXIT_SUCCESS;
		}
		else if (pid < 0) {
			while ((w_pid = waitpid(-1, &status, 0)) > 0) ;
				
			cout << "Parent exiting in failure" << endl;
			exit(EXIT_FAILURE);
		}
	}
	/* Parent(feeder) starts putting the numbers of the array in
	 * the shared memory, one by one, waiting each time
	 * for every child process to read it */
	for (int i = 0; i < array_size; ++i) {
		/* Time the request for the resource was made */
		struct timespec curr_time;
		clock_gettime(CLOCK_MONOTONIC, &curr_time);
		/* Write to the shared memory */
		shared_data->elem = M[i];
		shared_data->timestamp = curr_time;

		/* Awaken the consumers */
		Full.up(0, n_processes);
		Empty.up(0, n_processes);

		/* Wait for all consumers to finish reading before writing the next number */
		Empty.waitforzero(0);
	}
	while ((w_pid = waitpid(-1, &status, 0)) > 0) ;
	
	/* Detach and delete the shared memory segment */
	Shm.detach();
	Shm.clean();

	/* Remove the semaphores */
	Full.clean();
	Empty.clean();
	Mtx.clean();

	/* Free the used memory */
	delete [] M;

	/* Bye bye */
	cout << "Finished. Please check 'results_"<< args << ".log' for the full report and 'average_times_ "<< args << ".log' for a more simplified report.\n";
	return 0;
}

int validate_args(int argc, char **argv) {
	if (argc != 5) {
		fprintf(stderr, "Wrong arguments. Usage: ./feeder -n <no of C processes> -m <no of array elements>\n");
		return 0;
	}
	if (strcmp(argv[1],"-n") != 0 ) {
		fprintf( stderr, "Expected -n, got %s instead.\n", argv[1]);
		return 0;
	}
	if (strcmp(argv[3],"-m") != 0 ){
		fprintf( stderr, "Expected -m, got, got %s instead.\n", argv[3]);
		return 0;
	}
	return 1;
}