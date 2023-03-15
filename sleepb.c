#include <unistd.h>          //Provides API for POSIX(or UNIX) OS for system calls
#include <stdio.h>           //Standard I/O Routines
#include <stdlib.h>          //For exit() and rand()
#include <pthread.h>         //Threading APIs
#include <semaphore.h>       //Semaphore APIs
sem_t customers;                 //Semaphore
sem_t barbers;                   //Semaphore
sem_t mutex;  

#define MAX_CHAIRS 10        //No. of chairs in waiting room
#define CUT_TIME 1           //Hair Cutting Time 1 second
#define NUM_BARB 4           //No. of barbers
#define MAX_CUST 30          //Maximum no. of customers for simulation
                   //Semaphore for providing mutually exclusive access

int sitHereNext = 0;                  //Index for next available seat in queue
int nextCustomerToBeServed = 0;                  //Index to choose a candidate for cutting hair
int numberOfAvailableSeats = MAX_CHAIRS;   //counter for Vacant seats in waiting room
int queChair[MAX_CHAIRS];           //To exchange pid between customer and barber               
static int cnt = 0;                 //counter of No. of customers
void thread_Barber(void *ptr);         //Thread Function
void thread_Customer(void *ptr);       //Thread Function
void wait();           

int main()
{   
     
    pthread_t barber[NUM_BARB],customer[MAX_CUST]; //Thread declaration
    int itr,status=0;
    /*Semaphore initialization*/
    sem_init(&customers,0,0);
    sem_init(&barbers,0,0);
    sem_init(&mutex,0,1);   
    /*Barber thread initialization*/
    printf("!!....Barber Shop Opens...\n");
    for(itr=0;itr<NUM_BARB;itr++)                     //Creation of 2 Barber Threads
    {   
       status=pthread_create(&barber[itr],NULL,(void *)thread_Barber,(void*)&itr);
       sleep(1);
       if(status!=0)
          perror("No Barber Present... Sorry!!\n");
    }
    /*Customer thread initialization*/
    for(itr=0;itr<MAX_CUST;itr++)                     //Creation of Customer Threads
    {   
       status=pthread_create(&customer[itr],NULL,(void *)thread_Customer,(void*)&itr);
       wait();                   //Create customers in random interval
       if(status!=0)
           perror("No Customers Yet!!!\n");
    }   
    for(itr=0;itr<MAX_CUST;itr++)        //Waiting till all customers are dealt with
        pthread_join(customer[itr],NULL);
    printf("!!Barber Shop Closes!!\n");
    exit(EXIT_SUCCESS);  //Exit abandoning infinite loop of barber thread
}

void thread_Customer(void *ptr)  /*Customer Process*/
{   
    int mySeat, B;
    sem_wait(&mutex);  //Lock mutex to protect seat changes
    cnt++;           //Arrival of customer
    printf("Customer-%d[Id:%d] Entered Shop. ",cnt,pthread_self());
    if(numberOfAvailableSeats > 0) 
    {
        --numberOfAvailableSeats;           //Sit on chairs on waiting room
        printf("Customer-%d Sits In Waiting Room.\n",cnt);
        sitHereNext = (++sitHereNext) % MAX_CHAIRS;  //Choose a vacant chair to sit
        mySeat = sitHereNext;
        queChair[mySeat] = cnt;
        sem_post(&mutex);                  //Release the seat change mutex
        sem_post(&barbers);                //Wake up one barber
        sem_wait(&customers);              //Join queue of sleeping customers
        sem_wait(&mutex);                  //Lock mutex to protect seat changes
          B = queChair[mySeat];    //Barber replaces customer PID with his own PID
          numberOfAvailableSeats++;             //Stand Up and Go to Barber Room
        sem_post(&mutex);                        //Release the seat change mutex
                /*Customer is having hair cut by barber 'B'*/
    } 
    else 
    {
       sem_post(&mutex);  //Release the mutex and customer leaves without haircut
       printf("Customer-%d Finds No Seat & Leaves.\n",cnt);
    }
    pthread_exit(0);
}

void thread_Barber(void *ptr)        /*Barber Process*/
{   
    int pos = *(int *)(ptr);      
    int myNext, C;
    printf("Barber-%d[Id:%d] Joins Shop. ",pos,pthread_self());
    while(1)            /*Infinite loop*/   
    {   
        printf("Barber-%d Gone To Sleep.\n",pos);
        sem_wait(&barbers);          //Join queue of sleeping barbers
        sem_wait(&mutex);            //Lock mutex to protect seat changes
          nextCustomerToBeServed = (++nextCustomerToBeServed) % MAX_CHAIRS;  //Select next customer
          myNext = nextCustomerToBeServed;
          C = queChair[myNext];                  //Get selected customer's PID
          queChair[myNext] = pthread_self();     //Leave own PID for customer
        sem_post(&mutex);
        sem_post(&customers);         //Call selected customer
                /*Barber is cutting hair of customer 'C'*/
        printf("Barber-%d Wakes Up & Is Cutting Hair Of Customer-%d.\n",pos,C);
        sleep(CUT_TIME);
        printf("Barber-%d Finishes. ",pos);
    }
}

void wait()     /*Generates random number between 50000 to 250000*/
{
     int x = rand() % (250000 - 50000 + 1) + 50000; 
     srand(time(NULL));
     usleep(x);     //usleep halts execution in specified miliseconds
}