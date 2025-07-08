#include "bakery.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void run_baker(BakeryData* data, int semid, int employee_id) {
    srand(time(NULL) ^ getpid());
    Employee* me = &data->employees[employee_id];
    
    printf("Baker %d starting (Team: %d)\n", employee_id, me->team_id);

    while (data->simulation_running) {
        // Random delay to simulate work time
        usleep(500000 + rand() % 1000000); // 0.5-1.5 seconds
        
        semaphore_wait(semid, SEM_MUTEX);
        
        if (!data->simulation_running) {
            semaphore_signal(semid, SEM_MUTEX);
            break;
        }

        // Find first unbaked item that matches our team
        int item_to_bake = -1;
        for (int i = 0; i < data->item_count; i++) {
            if (!data->items[i].ready && data->items[i].team_id == me->team_id) {
                item_to_bake = i;
                break;
            }
        }

        if (item_to_bake == -1) {
         //   printf("Baker %d: No unbaked items found for team %d\n", 
                 // employee_id, me->team_id);
            semaphore_signal(semid, SEM_MUTEX);
            continue;
        }

        // Bake the item
        data->items[item_to_bake].ready = true;
        printf("Baker %d: Baked %s (ID:%d, Team:%d)\n", 
              employee_id, 
              data->items[item_to_bake].name,
              data->items[item_to_bake].id,
              data->items[item_to_bake].team_id);

        semaphore_signal(semid, SEM_BAKER); // Notify sellers
        semaphore_signal(semid, SEM_MUTEX);
    }

    printf("Baker Process %d (employee %d) exiting...\n", getpid(), employee_id);
}

