## bounded-buffer-solution

The problem involves developing a solution to the producer-consumer problem using semaphores and pthreads in the implementation of the solution. This involves ensuring a producer does not produce to a full buffer and a consumer does not consume from an empty buffer. Furthermore, we are asked to have five consumers consuming from the buffer all the while ensuring access to the buffer is mutually exclusive which involves only one thread accessing the buffer at a time to make changes to it. 

One example of this situation in the real world involves print jobs being sent to a communal printer. Several print jobs are sent to one printer which in turn “consumes” these print jobs one after the other. 
