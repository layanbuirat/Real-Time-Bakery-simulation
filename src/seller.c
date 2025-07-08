#include "bakery.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void run_seller(BakeryData* data, int semid, int employee_id) {
    while (data->simulation_running) {
        semaphore_wait(semid, SEM_MUTEX);
        
        // Track service activity
        int served_this_cycle = 0;
        
        // Find oldest waiting customer
        int customer_to_serve = -1;
        time_t oldest_waiting = time(NULL);
        
        for (int i = 0; i < data->customer_count; i++) {
            if (!data->customers[i].is_served && 
                !data->customers[i].is_frustrated &&
                data->customers[i].arrival_time < oldest_waiting) {
                customer_to_serve = i;
                oldest_waiting = data->customers[i].arrival_time;
            }
        }
        
        if (customer_to_serve == -1) {
            semaphore_signal(semid, SEM_MUTEX);
            usleep(500000);
            continue;
        }
        
        // Get requested item details
        int requested_item = data->customers[customer_to_serve].requested_item_id;
        const char* item_name = "Unknown";
        int item_index = -1;
        int available_quantity = 0;
        
        // Find available quantity
        for (int j = 0; j < data->item_count; j++) {
            if (data->items[j].id == requested_item) {
                item_name = data->items[j].name;
                if (data->items[j].ready && data->items[j].quantity > 0) {
                    item_index = j;
                    available_quantity = data->items[j].quantity;
                }
                break;
            }
        }
        
        // Serve customer if item available
        if (item_index != -1 && available_quantity > 0) {
            data->items[item_index].quantity--;
            data->total_profit += data->items[item_index].price;
            data->customers[customer_to_serve].is_served = true;
            served_this_cycle++;
            
            // 15% chance of complaint (adjust probability as needed)
            if ((rand() % 100) < 10) {
                data->customers[customer_to_serve].has_complained = true;
                data->complaining_customers++;
                data->total_profit -= data->items[item_index].price; // Refund
                
               printf("\033[38;2;255;165;0mSeller %d: SERVED customer %d with %s (Qty left: %d) BUT THEY COMPLAINED! Refunded $%.2f\033[0m\n",
       employee_id, customer_to_serve,
       item_name, data->items[item_index].quantity,
       data->items[item_index].price);

            } else {
               printf("\033[0;34mSeller %d: SERVED customer %d with %s (Qty left: %d)\033[0m\n",
       employee_id, customer_to_serve,
       item_name, data->items[item_index].quantity);

            }
        }
        else {
            // Check if waited too long
            if (time(NULL) - data->customers[customer_to_serve].arrival_time > data->simulation_time_limit/3) {
                data->customers[customer_to_serve].is_frustrated = true;
                data->frustrated_customers++;
                
                printf("Seller %d: CUSTOMER %d FRUSTRATED waiting for %s (waited %ld sec)\n",
                      employee_id, customer_to_serve, item_name,
                      time(NULL) - data->customers[customer_to_serve].arrival_time);
            }
        }
        
        semaphore_signal(semid, SEM_MUTEX);
        
        // Dynamic delay - faster when busy
        usleep(served_this_cycle ? 100000 : 500000);
    }
}





/*void run_seller(BakeryData* data, int semid, int employee_id) {
    srand(time(NULL) ^ getpid());
    
    while (data->simulation_running) {
        // Reduced sleep time to serve customers faster
        usleep(500000 + rand() % 1000000); // 0.5-1.5 second delay
        
        semaphore_wait(semid, SEM_MUTEX);
        
        if (!data->simulation_running) {
            semaphore_signal(semid, SEM_MUTEX);
            break;
        }
        
        // Find oldest waiting customer (FIFO)
        int customer_to_serve = -1;
        time_t oldest_waiting = time(NULL);
        for (int i = 0; i < data->customer_count; i++) {
            if (!data->customers[i].is_served && 
                !data->customers[i].is_frustrated) {
                if (customer_to_serve == -1 || 
                    data->customers[i].arrival_time < oldest_waiting) {
                    customer_to_serve = i;
                    oldest_waiting = data->customers[i].arrival_time;
                }
            }
        }
        
        if (customer_to_serve == -1) {
            semaphore_signal(semid, SEM_MUTEX);
            continue;
        }
        
        int requested_item_id = data->customers[customer_to_serve].requested_item_id;
        
        // Check ALL instances of the requested item
        int items_available = 0;
        for (int i = 0; i < data->item_count; i++) {
            if (data->items[i].id == requested_item_id && 
                data->items[i].ready && 
                data->items[i].quantity > 0) {
                items_available += data->items[i].quantity;
            }
        }
        
        if (items_available == 0) {
          //  printf("Seller %d: Item %d out of stock for customer %d\n",
                //  employee_id, requested_item_id, customer_to_serve);
            semaphore_signal(semid, SEM_MUTEX);
            continue;
        }
        
        // Find the item with largest quantity (prevent fragmentation)
        int best_item_idx = -1;
        int max_quantity = 0;
        for (int i = 0; i < data->item_count; i++) {
            if (data->items[i].id == requested_item_id &&
                data->items[i].ready &&
                data->items[i].quantity > max_quantity) {
                best_item_idx = i;
                max_quantity = data->items[i].quantity;
            }
        }
        
        // Complete the sale
        data->items[best_item_idx].quantity--;
        data->total_profit += data->items[best_item_idx].price;
        data->customers[customer_to_serve].is_served = true;
        
        printf("Seller %d: Sold %s (ID:%d Qty:%d) to customer %d (waited %ld sec) for $%.2f\n",
              employee_id, 
              data->items[best_item_idx].name,
              data->items[best_item_idx].id,
              data->items[best_item_idx].quantity,
              customer_to_serve,
              time(NULL) - data->customers[customer_to_serve].arrival_time,
              data->items[best_item_idx].price);
        
        // 10% chance of complaint
        if (rand() % 10 == 0) {
            data->customers[customer_to_serve].has_complained = true;
            data->complaining_customers++;
            data->total_profit -= data->items[best_item_idx].price;
            printf("Seller %d: Customer %d complained! Refunded $%.2f\n",
                  employee_id, customer_to_serve, data->items[best_item_idx].price);
        }
        
        semaphore_signal(semid, SEM_SELLER);
        semaphore_signal(semid, SEM_MUTEX);
    }
    
    printf("Seller Process %d (employee %d) exiting...\n", getpid(), employee_id);
}*/
