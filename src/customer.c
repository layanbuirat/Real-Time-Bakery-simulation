#include "bakery.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
void run_customer(BakeryData* data, int semid, int customer_id) {
    srand(time(NULL) ^ (getpid() << 16));

    semaphore_wait(semid, SEM_MUTEX);

    Customer* me = &data->customers[customer_id];
    me->id = customer_id;
    me->requested_item_id = rand() % 18; // Random item between 0-17
    me->is_served = false;
    me->is_frustrated = false;
    me->has_complained = false;
    me->arrival_time = time(NULL);

    const char* item_name = "Unknown Item";
    for (int i = 0; i < data->item_count; i++) {
        if (data->items[i].id == me->requested_item_id) {
            item_name = data->items[i].name;
            break;
        }
    }

    printf("\033[0;32mCustomer %d arrived - wants %s (ID:%d)\033[0m\n", 
           customer_id, item_name, me->requested_item_id);

    // Notify sellers
    semaphore_signal(semid, SEM_CUSTOMER);
    semaphore_signal(semid, SEM_MUTEX);

    // Wait to be served or leave frustrated
    time_t start_time = time(NULL);
    bool exited = false;

    while (!exited && data->simulation_running) {
        sleep(1);

        semaphore_wait(semid, SEM_MUTEX);

        if (me->is_served) {
            if (me->has_complained) {
                printf("\033[0;33mCustomer %d was served but complained about %s (ID:%d)\033[0m\n",
                      customer_id, item_name, me->requested_item_id);
            } else {
                printf("\033[0;32mCustomer %d successfully got %s (ID:%d)\033[0m\n",
                      customer_id, item_name, me->requested_item_id);
            }
            semaphore_signal(semid, SEM_MUTEX);
            exited = true;
            continue;
        }

        if (me->is_frustrated) {
            printf("\033[0;31mCustomer %d left frustrated without %s (ID:%d)\033[0m\n",
                  customer_id, item_name, me->requested_item_id);
            semaphore_signal(semid, SEM_MUTEX);
            exited = true;
            continue;
        }

        // Check waiting timeout
        if (time(NULL) - start_time > data->simulation_time_limit/3) { // Shorter timeout (was 100)
            if (!me->is_served) {
                me->is_frustrated = true;
                data->frustrated_customers++;
                printf("\033[0;31mCustomer %d waited too long for %s (ID:%d) - leaving frustrated\033[0m\n",
                      customer_id, item_name, me->requested_item_id);
                semaphore_signal(semid, SEM_MUTEX);
                exited = true;
                continue;
            }
        }

        // Extra push (OPTIONAL TRICK)
        // Resignal SEM_CUSTOMER if still not served every 10 seconds
        if (((time(NULL) - start_time) % 10) == 0) {
            semaphore_signal(semid, SEM_CUSTOMER);
        }

        semaphore_signal(semid, SEM_MUTEX);
    }

    printf("Customer Process %d (customer %d) exiting...\n", getpid(), customer_id);
}


/*void run_customer(BakeryData* data, int semid, int customer_id) {
    // Initialize random seed unique to this customer process
    srand(time(NULL) ^ (getpid() << 16));
    
    // Wait for mutex before accessing shared data
    semaphore_wait(semid, SEM_MUTEX);
    
    // Set up customer information
    Customer* me = &data->customers[customer_id];
    me->id = customer_id;
    me->requested_item_id = rand() % 18; // Random item between 0-17
    me->is_served = false;
    me->is_frustrated = false;
    me->has_complained = false;
    me->arrival_time = time(NULL);
    
    // Get item name for display (if available)
    const char* item_name = "Unknown Item";
    for (int i = 0; i < data->item_count; i++) {
        if (data->items[i].id == me->requested_item_id) {
            item_name = data->items[i].name;
            break;
        }
    }
    
    printf("\033[0;32mCustomer %d arrived - wants %s (ID:%d)\033[0m\n", 
           customer_id, item_name, me->requested_item_id);
    
    // Notify sellers that a new customer is waiting
    semaphore_signal(semid, SEM_CUSTOMER);
    semaphore_signal(semid, SEM_MUTEX);
    
    // Wait to be served or get frustrated
    time_t start_time = time(NULL);
    bool exited = false;
    
    while (!exited && data->simulation_running) {
        sleep(1); // Check status every second
        
        semaphore_wait(semid, SEM_MUTEX);
        
        // Check if we've been served
        if (me->is_served) {
            if (me->has_complained) {
                printf("\033[0;33mCustomer %d was served but complained about %s (ID:%d)\033[0m\n",
                      customer_id, item_name, me->requested_item_id);
            } else {
                printf("\033[0;32mCustomer %d successfully got %s (ID:%d)\033[0m\n",
                      customer_id, item_name, me->requested_item_id);
            }
            semaphore_signal(semid, SEM_MUTEX);
            exited = true;
            continue;
        }
        
        // Check if we've been marked as frustrated
        if (me->is_frustrated) {
            printf("\033[0;31mCustomer %d left frustrated without %s (ID:%d)\033[0m\n",
                  customer_id, item_name, me->requested_item_id);
            semaphore_signal(semid, SEM_MUTEX);
            exited = true;
            continue;
        }
        
        // Check if we've been waiting too long (30 seconds)
        if (time(NULL) - start_time > 100) {
            me->is_frustrated = true;
            data->frustrated_customers++;
            printf("\033[0;31mCustomer %d waited too long for %s (ID:%d) - leaving frustrated\033[0m\n",
                  customer_id, item_name, me->requested_item_id);
            semaphore_signal(semid, SEM_MUTEX);
            exited = true;
            continue;
        }
        
        // Still waiting...
       // printf("Customer %d still waiting for %s (ID:%d) [%ld seconds]\n",
         //     customer_id, item_name, me->requested_item_id, 
           //   time(NULL) - start_time);
        
        semaphore_signal(semid, SEM_MUTEX);
    }
    
    printf("Customer Process %d (customer %d) exiting...\n", getpid(), customer_id);
}*/
