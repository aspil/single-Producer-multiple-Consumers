# single-Producer-multiple-Consumers
The solution was constructed under the limitation that the shared memory contains only one element(the value to be passed to consumers) and a timestamp of the moment that element was produced. That means, no shared counters can be used.

One way to solve this, is to use semaphores for each consumer.  
However, the problem can be solved in a more "eco-friendly" way.

## The solution
We're using two semaphores Empty and Full to handle the synchronization, both initialized to 0.
If a consumer attempts to consume the shared memory segment, it will be blocked.
#### Producer
The producer writes a random integer from the local array to the shared memory and performs an instant *up* operation on both semaphores by a value equal to the number of the total consumers. Then, the producer performs a wait-for-zero operation on the Empty semaphore, and gets blocked until the semaphore's value becomes zero.  
Semantically, this means that the producer will remain blocked until all the consumers have consumed the element of the shared memory segment.

#### Consumer
The consumers remain blocked or in queue before the producer until the up operations.  
When the producer performs the *up* operations, the consumers will start to consume until they fill their local arrays.  
Each one performs a down operation on the Full semaphore, consume the element from the shared memory segment, do some calculations and then perform a wait-for-zero operation on the Full semaphore. That way all Consumers will block until the semaphore Full becomes 0 and then each one performs a *down* operation on the Empty semaphore.  
The last one will unblock the Producer that is waiting for the Empty semaphore to become 0.
