# Sleeping Barber Problem

## Problem Statement:

There are multiple barbers in a shop having fixed number of waiting chairs. In idle state , barbers go in sleep mode.There are multiple customers which have to be alloted to barbers depending upon availability & arrival time.We need a synchronization such that system is in safe state.

## Solution:

Our solution ensures that a barber *(a thread)* sleeps when there are no customers *(each customer is a thread)* in the waiting room  \*\*. If there is someone, he cuts his hair (symbolizing some processing) and the customer leaves. If, then, there is no one in the waiting room, the barber goes to sleep. When another customer arrives, he then must wake up the barber.This must be synchronised for all barber & customers threads.

We have used two semaphores **barbers & clients** both initialized to zero. An additional semaphore **mutex** ensures mutual exclusion among threads.We have maintained a queue of waiting customers using **queChair** .

Until availableSeats is greater than zero , customers is assigned to waiting queue otherwise customers leaves.Whenever a barber thread is released it checks for waiting customer in queue before going to sleep state.
