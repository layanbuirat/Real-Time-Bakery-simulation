#include "bakery.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

const int INGREDIENT_THRESHOLDS[] = {2, 1, 1, 1, 2, 3, 3};

const char* ingredient_name(int index) {
    const char* names[] = {
        "Wheat", "Yeast", "Butter", "Milk",
        "Sugar and Salt", "Sweet Items", "Cheese and Salami"
    };
    return (index >= 0 && index < 7) ? names[index] : "Unknown";
}

void run_supply_chain(BakeryData* data, int semid) {
    srand(time(NULL) ^ getpid());
    
    while (data->simulation_running) {
        sleep(5);
        
        semaphore_wait(semid, SEM_MUTEX);
        
        if (!data->simulation_running) {
            semaphore_signal(semid, SEM_MUTEX);
            break;
        }
        
        for (int i = 0; i < 7; i++) {
            if (data->ingredients[i].quantity < INGREDIENT_THRESHOLDS[i]) {
                int restock_amount = (rand() % 21) + 30;
                data->ingredients[i].quantity = restock_amount;
                data->ingredients[i].available = true;
                printf("Supply Chain: Restocked %s to %d units\n",
                      ingredient_name(i),
                      data->ingredients[i].quantity);
            }
        }
        
        semaphore_signal(semid, SEM_SUPPLY);
        semaphore_signal(semid, SEM_MUTEX);
    }
    
    printf("Supply Chain Process %d exiting...\n", getpid());
}
