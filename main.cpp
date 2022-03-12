/***************************************************************************
*
* File: main.cpp
* Author: David Kumar (DDK170002)
* Procedures:
*   void *Producer(void *arg) - this function creates the producer which in turn appends an item to the buffer
*   void *Consumer(void *arg) - this function creates the consumer which in turn consumes an item from the buffer
*   void exportCSV(std::map<int, int> &consumer_data) - this map contains the key-value mapping of the consumer number and the number of values consumed
*   int main( int argc, char *argv[] ) - this driver function creates five threads (4 producers and 1 consumer) which in turn produces and consumes data from the buffer
*
***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> // POSIX threads within the same process
#include <semaphore.h> // POSIX semaphores to allow processes and threads to synchronize
#include <poll.h> // adding timeout to a process
#include <fstream> // adding library to create CSV files
#include <map> // adding to use std::map for key-value pairs

void *Producer();
void *Consumer();

sem_t s, n; // declaring two semaphores
long long buffer[999999]; // somewhat the equivalent of an 'infinite' buffer
int index_buffer = 0;  // acts as an index pointer to the next value of the buffer  
int consumer_count = 0; // assigning count to each consumer (1->5 consumers)

/***************************************************************************
*
* void *Producer(void *arg)
* Author: David Kumar
* Due: 03/11/2022
* Description: this function creates the producer which in turn appends an item to the buffer
* Parameters:
*   void *arg - this is the pointer to the thread reference from the driver function
* Return:
*   void* - return type
*
**************************************************************************/
void *Producer(void *arg){
   
    long long produced_count = 0;
    printf("\nProducer created with ID: %ld\n", pthread_self());
   
    while(true){
       
        produced_count++;    // incrementing the counter to denote the production of an item
        sem_wait(&s); // locking the semaphore
        buffer[produced_count-1] = produced_count;  // appending the new value to the buffer
        poll(0,0,500);    // throttling the data in the critical section to ensure not too many data points are produced in the graph
        sem_post(&s); // unlocking the semaphore
        printf("\nProducer creates item #%lld\n", produced_count);
        sem_post(&n); // unlocking the semaphore
       
    }
   
    return NULL;
}

/***************************************************************************
*
* void *Consumer(void *arg)
* Author: David Kumar
* Due: 03/11/2022
* Description: this function creates the consumer which in turn consumes an item from the buffer
* Parameters:
*   void *arg - this is the pointer to the thread reference from the driver function
* Return:
*   void* - return type
*
**************************************************************************/
void *Consumer(void *arg){
   
    long long total_consumed = 0; // tracks the total number of items consumed from the buffer
    int* consumed = (int*) arg;   // tracks the number of items consumed from the buffer per consumer
    int thread_count = ++consumer_count; // tracks the current consumer number
    printf("\nConsumer created with ID: %ld\n",pthread_self());

    while(true){
       
        sem_wait(&n); // locking the semaphore
        sem_wait(&s); // locking the semaphore
        total_consumed = buffer[index_buffer++];   // consuming an item from the buffer
        sem_post(&s); // unlocking the semaphore
       
        printf("\nConsumer# %d has consumed %d items out of a total of %lld items\n", thread_count, ++(*consumed), total_consumed);
       
    }

    return NULL;
}

/***************************************************************************
*
* void exportCSV(std::map<int, int> &consumer_data)
* Author: David Kumar
* Due: 03/11/2022
* Description: this function facilitates the creation of a CSV file to plot the number of values consumed by which consumer
* Parameters:
*   std::map<int, int> &consumer_data - this map contains the key-value mapping of the consumer number and the number of values consumed
* Return:
*   void - return type
*
**************************************************************************/
void exportCSV(std::map<int, int> &consumer_data){
   
    std::ofstream file; // initializing the file
    file.open("result.csv"); // opening the file
    for (auto &row:consumer_data){ // iterating through the map which contains the key-value mapping of the consumer number and the number of values consumed
        std::string each_row = std::to_string(row.first) + "," + std::to_string(row.second) + "\n";
        file << each_row;
    }
   
    file.close(); // closing the file to ensure data doesn't corrupt
    return ;
   
}

/***************************************************************************
*
* int main( int argc, char *argv[] )
* Author: David Kumar
* Due: 03/11/2022
* Description: this driver function creates five threads (4 producers and 1 consumer) which in turn produces and consumes data from the buffer
* Parameters:
    * argc I/P int The number of arguments on the command line
    * argv I/P char *[] The arguments on the command line
    * main O/P int Status code (not currently used)
*
**************************************************************************/
int main(int argc, char *argv[]){
   
    pthread_t ptid, ctid1, ctid2, ctid3, ctid4, ctid5; // creating the pthread_t for the five threads
   
    int thread_reference1 = 0; // value passed to Consumer 1
    int thread_reference2 = 0; // value passed to Consumer 2
    int thread_reference3 = 0; // value passed to Consumer 3
    int thread_reference4 = 0; // value passed to Consumer 4
    int thread_reference5 = 0; // value passed to Consumer 5
   
    sem_init(&s, 1, 1); // initializing an unnamed semaphore that is shared between processes, with a starting value of 1
    sem_init(&n, 1, 0); // initializing an unnamed semaphore that is shared between processes, with a starting value of 0

    pthread_create(&ptid, NULL, Producer, NULL); // creating a pthread for the producer
    pthread_create(&ctid1, NULL, Consumer, &thread_reference1); // creating a pthread for the first consumer
    pthread_create(&ctid2, NULL, Consumer, &thread_reference2); // creating a pthread for the second consumer
    pthread_create(&ctid3, NULL, Consumer, &thread_reference3); // creating a pthread for the third consumer
    pthread_create(&ctid4, NULL, Consumer, &thread_reference4); // creating a pthread for the fourth consumer
    pthread_create(&ctid5, NULL, Consumer, &thread_reference5); // creating a pthread for the fifth consumer
   
    poll(0,0,300000); // allowing the threads to produce and consume from the buffer for five minutes

    pthread_cancel(ptid); // sending a cancellation request to each of the below threads
    pthread_cancel(ctid1);
    pthread_cancel(ctid2);
    pthread_cancel(ctid3);
    pthread_cancel(ctid4);
    pthread_cancel(ctid5);

    printf("\nConsumer 1 consumed %d items", thread_reference1);
    printf("\nConsumer 2 consumed %d items", thread_reference2);
    printf("\nConsumer 3 consumed %d items", thread_reference3);
    printf("\nConsumer 4 consumed %d items", thread_reference4);
    printf("\nConsumer 5 consumed %d items", thread_reference5);
   
    std::map<int, int> consumer_data = {{1, thread_reference1}, // exporting consumer data into a map
                                {2, thread_reference2},
                                {3, thread_reference3},
                                {4, thread_reference4},
                                {5, thread_reference5}};
                               
    exportCSV(consumer_data); // creating a CSV for the different consumers
   
}