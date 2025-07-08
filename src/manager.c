#include "bakery.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void run_manager(BakeryData* data, int semid) {
    time_t start_time = time(NULL);
    time_t elapsed_seconds = 0;
    
    printf("Manager: Starting simulation (Time limit: %d seconds)\n", 
          data->simulation_time_limit);
    
    while (data->simulation_running) {
        sleep(1); // Check every second
        
        semaphore_wait(semid, SEM_MUTEX);
        
        // Calculate elapsed time
        elapsed_seconds = time(NULL) - start_time;
        data->bakery_time = elapsed_seconds / 60;

        // Check termination conditions
        if (elapsed_seconds >= data->simulation_time_limit) {
            data->simulation_running = false;
            printf("\033[0;35mManager: TIME LIMIT REACHED (%ld >= %d seconds)\033[0m\n",
                  elapsed_seconds, data->simulation_time_limit);
            semaphore_signal(semid, SEM_MUTEX);
            break;
        }
        if (data->frustrated_customers >= data->max_frustrated_customers) {
            data->simulation_running = false;
            printf("\033[0;35mManager: Too many frustrated customers (%d >= %d)\033[0m\n",
                  data->frustrated_customers, data->max_frustrated_customers);
            semaphore_signal(semid, SEM_MUTEX);
            break;
        }
        if (data->complaining_customers >= data->max_complaining_customers) {
            data->simulation_running = false;
            printf("\033[0;35mManager: Too many complaining customers (%d >= %d)\033[0m\n",
                  data->complaining_customers, data->max_complaining_customers);
            semaphore_signal(semid, SEM_MUTEX);
            break;
        }
        if (data->missing_item_customers >= data->max_missing_item_customers) {
            data->simulation_running = false;
            printf("\033[0;35mManager: Too many missing item requests (%d >= %d)\033[0m\n",
                  data->missing_item_customers, data->max_missing_item_customers);
            semaphore_signal(semid, SEM_MUTEX);
            break;
        }
        if (data->total_profit >= data->profit_threshold) {
            data->simulation_running = false;
            printf("\033[0;35mManager: Profit target reached (%.2f >= %.2f)\033[0m\n",
                  data->total_profit, data->profit_threshold);
            semaphore_signal(semid, SEM_MUTEX);
            break;
        }

        // Enhanced rebalancing logic
        int team_items[6] = {0}; // Tracks ready items per team (TEAM_PASTE to TEAM_SAVORY_PATISSERIES)
        int team_unbaked[6] = {0}; // Tracks unbaked items per team
        
        // Count items per team
        for (int i = 0; i < data->item_count; i++) {
            int team = data->items[i].team_id;
            if (team >= 0 && team < 6) { // Only count production teams (0-5)
                if (data->items[i].ready) {
                    team_items[team] += data->items[i].quantity;
                } else {
                    team_unbaked[team]++;
                }
            }
        }

        // 1. Rebalance chefs between production teams
        for (int from = 0; from < 6; from++) {
            for (int to = 0; to < 6; to++) {
                if (from != to && 
                    team_items[to] < 3 && // Target team has low inventory
                    team_items[from] > 10 && // Source team has excess
                    team_unbaked[to] > 2) { // Target team has unbaked items
                    
                    // Find first available chef in source team
                    for (int i = 0; i < data->employee_count; i++) {
                        if (data->employees[i].team_id == from && 
                            !data->employees[i].busy) {
                            data->employees[i].team_id = to;
                            printf("\033[0;36mManager: Rebalanced chef %d from team %d to %d\033[0m\n",
                                  i, from, to);
                            break;
                        }
                    }
                }
            }
        }

        // 2. Rebalance bakers - alternative approach without role check
        for (int team = 0; team < 6; team++) {
            if (team_unbaked[team] > 5) { // Critical backlog of unbaked items
                // Find any available employee who can bake
                for (int i = 0; i < data->employee_count; i++) {
                    // Check if employee is not busy and is assigned to a baking team
                    // (Assuming baking teams have IDs >= 6)
                    if (!data->employees[i].busy && 
                        data->employees[i].team_id >= 6) {
                        data->employees[i].team_id = team;
                        printf("\033[0;36mManager: Reassigned employee %d to team %d (%d unbaked)\033[0m\n",
                              i, team, team_unbaked[team]);
                        break;
                    }
                }
            }
        }

        semaphore_signal(semid, SEM_MUTEX);
    }
    
    printf("Manager: Simulation ended after %ld seconds\n", time(NULL) - start_time);
    printf("Manager Process %d exiting...\n", getpid());
}
