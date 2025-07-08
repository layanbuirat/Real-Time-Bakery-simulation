#include "bakery.h"
#include "visualization.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]) {
    
    
     FILE *file = fopen("variables.txt", "r");
    if (!file) {
        perror("Failed to open variables.txt");
        return 1;
    }

  int num_chefs, num_bakers, num_sellers, num_supply_chain;
int max_customers;
int simulation_time_limit;
int max_frustrated_customers;
int max_complaining_customers;
int max_missing_item_customers;
int simulation_running;
float profit_threshold;


    // Read the file line by line
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char key[50];
        char value[50];
        
        if (sscanf(line, "%[^=]=%s", key, value) == 2) {
            if (strcmp(key, "num_chefs") == 0) {
                num_chefs = atoi(value);
            } else if (strcmp(key, "num_bakers") == 0) {
                num_bakers = atoi(value);
            } else if (strcmp(key, "num_sellers") == 0) {
                num_sellers = atoi(value);
            } else if (strcmp(key, "num_supply_chain") == 0) {
                num_supply_chain = atoi(value);
            } else if (strcmp(key, "max_customers") == 0) {
                max_customers = atoi(value);
            } else if (strcmp(key, "profit_threshold") == 0) {
                profit_threshold = atof(value);
            } else if (strcmp(key, "time_limit") == 0) {
                simulation_time_limit = atoi(value);
            } else if (strcmp(key, "max_frustrated_customers") == 0) {
                max_frustrated_customers = atoi(value);
            } else if (strcmp(key, "max_complaining_customers") == 0) {
                max_complaining_customers = atoi(value);
            } else if (strcmp(key, "max_missing_item_customers") == 0) {
                max_missing_item_customers = atoi(value);
            } else if (strcmp(key, "simulation_time_limit") == 0) {
                simulation_time_limit = atoi(value);
            } else if (strcmp(key, "simulation_running") == 0) {
                simulation_running = (strcmp(value, "true") == 0) ? 1 : 0;
            }
        }
    }
    fclose(file);



    

    
    // Create shared memory and semaphores
    int shmid = create_shared_memory();
    int semid = create_semaphores();
    
    BakeryData* data = attach_shared_memory(shmid);
    
       // Initialize bakery data
    initialize_bakery(data);
    for (int i = 0; i < 18; i++) {
    printf("Item %d: %s, Quantity: %d\n", 
           i, data->items[i].name, data->items[i].quantity);
}
    
   
     
    data->max_frustrated_customers = max_frustrated_customers;
    data->max_complaining_customers = max_complaining_customers;
    data->max_missing_item_customers = max_missing_item_customers;
    data->profit_threshold = profit_threshold;
    data->simulation_time_limit = simulation_time_limit;
    data->simulation_running = simulation_running;
    
    
    // Check if all values are assigned correctly
printf("=== Simulation Configuration ===\n");
printf("num_chefs: %d\n", num_chefs);
printf("num_bakers: %d\n", num_bakers);
printf("num_sellers: %d\n", num_sellers);
printf("num_supply_chain: %d\n", num_supply_chain);
printf("max_customers: %d\n", max_customers);
printf("max_frustrated_customers: %d\n", max_frustrated_customers);
printf("max_complaining_customers: %d\n", max_complaining_customers);
printf("max_missing_item_customers: %d\n", max_missing_item_customers);
printf("profit_threshold: %.2f\n", profit_threshold);
printf("simulation_time_limit: %d\n", simulation_time_limit);
printf("simulation_running: %d\n", simulation_running);
printf("=================================\n");

    
    // Create teams
    data->teams[TEAM_PASTE] = (Team){TEAM_PASTE, "Paste Team", 0, 0, true};
    data->teams[TEAM_CAKES] = (Team){TEAM_CAKES, "Cakes Team", 0, 0, true};
    data->teams[TEAM_SANDWICHES] = (Team){TEAM_SANDWICHES, "Sandwiches Team", 0, 0, true};
    data->teams[TEAM_SWEETS] = (Team){TEAM_SWEETS, "Sweets Team", 0, 0, true};
    data->teams[TEAM_SWEET_PATISSERIES] = (Team){TEAM_SWEET_PATISSERIES, "Sweet Patisseries Team", 0, 0, true};
    data->teams[TEAM_SAVORY_PATISSERIES] = (Team){TEAM_SAVORY_PATISSERIES, "Savory Patisseries Team", 0, 0, true};
    data->teams[TEAM_BAKE_CAKES_SWEETS] = (Team){TEAM_BAKE_CAKES_SWEETS, "Bake Cakes/Sweets Team", 0, 0, true};
    data->teams[TEAM_BAKE_PATISSERIES] = (Team){TEAM_BAKE_PATISSERIES, "Bake Patisseries Team", 0, 0, true};
    data->teams[TEAM_BAKE_BREAD] = (Team){TEAM_BAKE_BREAD, "Bake Bread Team", 0, 0, true};
    data->team_count = 9;
    
    // Create employees
    data->employee_count = 0;
    
    // Chefs
    for (int i = 0; i < num_chefs; i++) {
        int team_id = TEAM_PASTE + (i % 6); // Distribute chefs across teams
        data->employees[data->employee_count] = (Employee){
            data->employee_count, "Chef", team_id, false
        };
        data->teams[team_id].employee_count++;
        data->employee_count++;
    }
    
    // Bakers
   // In main.c, modify the baker creation loop
for (int i = 0; i < num_bakers; i++) {
    int team_id;
    switch (i % 3) {
        case 0: team_id = TEAM_PASTE; break; // For bread
        case 1: team_id = TEAM_CAKES; break; // For cakes
        case 2: team_id = TEAM_SWEETS; break; // For sweets
    }
    data->employees[data->employee_count] = (Employee){
        data->employee_count, "Baker", team_id, false
    };
    data->teams[team_id].employee_count++;
    data->employee_count++;
}
    
    // Sellers
    for (int i = 0; i < num_sellers; i++) {
        data->employees[data->employee_count] = (Employee){
            data->employee_count, "Seller", -1, false // Sellers don't belong to a team
        };
        data->employee_count++;
    }
    
    // Initialize visualization in a separate process
    pid_t viz_pid = fork();
    if (viz_pid == 0) {
        init_visualization(argc, argv, data, semid);
        glutMainLoop();
        exit(0);
    }
    
    // Fork other processes
    pid_t pid;
    
    // Supply chain processes
    for (int i = 0; i < num_supply_chain; i++) {
        pid = fork();
        if (pid == 0) {
            run_supply_chain(data, semid);
            exit(0);
        }
    }
    
    // Chef processes
    for (int i = 0; i < num_chefs; i++) {
        pid = fork();
        if (pid == 0) {
            run_chef(data, semid, i);
            exit(0);
        }
    }
    
    // Baker processes
    for (int i = 0; i < num_bakers; i++) {
        pid = fork();
        if (pid == 0) {
            run_baker(data, semid, num_chefs + i); // Employee IDs continue from chefs
            exit(0);
        }
    }
    
    // Seller processes
    for (int i = 0; i < num_sellers; i++) {
        pid = fork();
        if (pid == 0) {
            run_seller(data, semid, num_chefs + num_bakers + i);
            exit(0);
        }
    }
    
    // Manager process
    pid = fork();
    if (pid == 0) {
        run_manager(data, semid);
        exit(0);
    }
    
    // Customer processes (spawned periodically)
    int customer_counter = 0;
    while (data->simulation_running) {
         int sleep_time = 4 + rand() % 4;
         sleep(sleep_time);
        
        semaphore_wait(semid, SEM_MUTEX);
        
        if (!data->simulation_running || customer_counter >= max_customers) {
            semaphore_signal(semid, SEM_MUTEX);
            break;
        }
        
        if (data->customer_count < MAX_CUSTOMERS) {
            pid = fork();
            if (pid == 0) {
                run_customer(data, semid, data->customer_count);
                exit(0);
            } else {
                data->customer_count++;
                customer_counter++;
            }
        }
        
        semaphore_signal(semid, SEM_MUTEX);
    }
    
    // Wait for all child processes to finish
    while (wait(NULL) > 0);
    
    // Clean up
    detach_shared_memory(data);
    destroy_shared_memory(shmid);
    destroy_semaphores(semid);
    
    return 0;
}
