#include "bakery.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void run_chef(BakeryData* data, int semid, int employee_id) {
    srand(time(NULL) ^ getpid());
    Employee* me = &data->employees[employee_id];
    Team* my_team = &data->teams[me->team_id];
    
    while (data->simulation_running) {
        sleep(1 + rand() % 3); // Simulate work time
        
        semaphore_wait(semid, SEM_MUTEX);
        
        if (!data->simulation_running) {
            semaphore_signal(semid, SEM_MUTEX);
            break;
        }
        
        // Check dependencies based on team requirements
        bool can_work = true;
        
        switch (me->team_id) {
            case TEAM_SWEET_PATISSERIES:
            case TEAM_SAVORY_PATISSERIES:
                // Depend on paste team's products (bread)
                can_work = (data->teams[TEAM_PASTE].items_prepared > 0) &&
                           data->ingredients[WHEAT].available && 
                           data->ingredients[YEAST].available &&
                           data->ingredients[WHEAT].quantity > 0 &&
                           data->ingredients[YEAST].quantity > 0;
                if (can_work) {
                    data->teams[TEAM_PASTE].items_prepared--; // Use one bread product
                }
                break;
                
            case TEAM_SANDWICHES:
                // Depend on bread team's products
                can_work = (data->teams[TEAM_PASTE].items_prepared > 0) &&
                           data->ingredients[CHEESE_SALAMI].available &&
                           data->ingredients[CHEESE_SALAMI].quantity > 2;
                if (can_work) {
                    data->teams[TEAM_PASTE].items_prepared--; // Use one bread product
                }
                break;
                
            case TEAM_CAKES:
                can_work = data->ingredients[SUGAR_SALT].available &&
                           data->ingredients[SWEET_ITEMS].available &&
                           data->ingredients[MILK].available &&
                           data->ingredients[BUTTER].available &&
                           data->ingredients[SUGAR_SALT].quantity > 1 &&
                           data->ingredients[SWEET_ITEMS].quantity > 2 &&
                           data->ingredients[MILK].quantity > 0 &&
                           data->ingredients[BUTTER].quantity > 0;
                break;
                
            case TEAM_SWEETS:
                can_work = data->ingredients[SUGAR_SALT].available &&
                           data->ingredients[SWEET_ITEMS].available &&
                           data->ingredients[BUTTER].available &&
                           data->ingredients[SUGAR_SALT].quantity > 0 &&
                           data->ingredients[SWEET_ITEMS].quantity > 1 &&
                           data->ingredients[BUTTER].quantity > 0;
                break;
                
            case TEAM_PASTE: // Bread team
                can_work = data->ingredients[WHEAT].available &&
                           data->ingredients[YEAST].available &&
                           data->ingredients[WHEAT].quantity > 1 &&
                           data->ingredients[YEAST].quantity > 0;
                break;
        }
        
        if (!can_work) {
            //printf("Chef %d (Team %d): Waiting for ingredients or dependencies\n", employee_id, me->team_id);
            semaphore_signal(semid, SEM_MUTEX);
            continue;
        }
        
        // Create a new item based on team
        if (data->item_count < MAX_ITEMS) {
            BakeryItem new_item;
            new_item.id = data->item_count;
            new_item.quantity = 1;
            new_item.team_id = me->team_id;
            new_item.ready = false; // Needs to be baked
            
            switch (me->team_id) {
                case TEAM_PASTE: // Bread Team
                    switch (rand() % 3) {
                        case 0: 
                            strcpy(new_item.name, "White Bread");
                            new_item.price = WHITE_BREAD_PRICE;
                            break;
                        case 1:
                            strcpy(new_item.name, "Whole Wheat Bread");
                            new_item.price = WHOLE_WHEAT_BREAD_PRICE;
                            break;
                        case 2:
                            strcpy(new_item.name, "Burger Buns");
                            new_item.price = BURGER_BUNS_PRICE;
                            break;
                    }
                    data->ingredients[WHEAT].quantity -= 2;
                    data->ingredients[YEAST].quantity -= 1;
                    data->teams[TEAM_PASTE].items_prepared++; // Track bread products made
                    break;
                    
                case TEAM_SANDWICHES:
                    switch (rand() % 3) {
                        case 0:
                            strcpy(new_item.name, "Cheese Sandwich");
                            new_item.price = CHEESE_SANDWICH_PRICE;
                            break;
                        case 1:
                            strcpy(new_item.name, "Salami Sandwich");
                            new_item.price = SALAMI_SANDWICH_PRICE;
                            break;
                        case 2:
                            strcpy(new_item.name, "Cheese and Salami Sandwich");
                            new_item.price = CHEESE_SALAMI_SANDWICH_PRICE;
                            break;
                    }
                    data->ingredients[CHEESE_SALAMI].quantity -= 3;
                    break;
                    
                case TEAM_CAKES:
                    switch (rand() % 3) {
                        case 0:
                            strcpy(new_item.name, "Vanilla Cake");
                            new_item.price = VANILLA_CAKE_PRICE;
                            break;
                        case 1:
                            strcpy(new_item.name, "Chocolate Cake");
                            new_item.price = CHOCOLATE_CAKE_PRICE;
                            break;
                        case 2:
                            strcpy(new_item.name, "Strawberry Cake");
                            new_item.price = STRAWBERRY_CAKE_PRICE;
                            break;
                    }
                    data->ingredients[SUGAR_SALT].quantity -= 2;
                    data->ingredients[SWEET_ITEMS].quantity -= 3;
                    data->ingredients[MILK].quantity -= 1;
                    data->ingredients[BUTTER].quantity -= 1;
                    break;
                    
                case TEAM_SWEETS:
                    switch (rand() % 3) {
                        case 0:
                            strcpy(new_item.name, "Donuts");
                            new_item.price = DONUTS_PRICE;
                            break;
                        case 1:
                            strcpy(new_item.name, "Cupcakes");
                            new_item.price = CUPCAKES_PRICE;
                            break;
                        case 2:
                            strcpy(new_item.name, "Brownies");
                            new_item.price = BROWNIES_PRICE;
                            break;
                    }
                    data->ingredients[SUGAR_SALT].quantity -= 1;
                    data->ingredients[SWEET_ITEMS].quantity -= 2;
                    data->ingredients[BUTTER].quantity -= 1;
                    break;
                    
                case TEAM_SWEET_PATISSERIES:
                    switch (rand() % 3) {
                        case 0:
                            strcpy(new_item.name, "Cream Rolls");
                            new_item.price = CREAM_ROLLS_PRICE;
                            break;
                        case 1:
                            strcpy(new_item.name, "Jam Tarts");
                            new_item.price = JAM_TARTS_PRICE;
                            break;
                        case 2:
                            strcpy(new_item.name, "Mini Muffins");
                            new_item.price = MINI_MUFFINS_PRICE;
                            break;
                    }
                    data->ingredients[WHEAT].quantity -= 1;
                    data->ingredients[YEAST].quantity -= 1;
                    data->ingredients[SUGAR_SALT].quantity -= 1;
                    data->ingredients[SWEET_ITEMS].quantity -= 1;
                    break;
                    
                case TEAM_SAVORY_PATISSERIES:
                    switch (rand() % 3) {
                        case 0:
                            strcpy(new_item.name, "Cheese Puffs");
                            new_item.price = CHEESE_PUFFS_PRICE;
                            break;
                        case 1:
                            strcpy(new_item.name, "Mini Pizzas");
                            new_item.price = MINI_PIZZAS_PRICE;
                            break;
                        case 2:
                            strcpy(new_item.name, "Sausage Rolls");
                            new_item.price = SAUSAGE_ROLLS_PRICE;
                            break;
                    }
                    data->ingredients[WHEAT].quantity -= 1;
                    data->ingredients[YEAST].quantity -= 1;
                    data->ingredients[CHEESE_SALAMI].quantity -= 1;
                    break;
            }
            
            data->items[data->item_count] = new_item;
            data->item_count++;
            
            printf("Chef %d (Team %d): Created new %s (price: %.2f)\n", 
                  employee_id, me->team_id, new_item.name, new_item.price);
            
            // Check if we're out of any ingredients
            for (int i = 0; i < 7; i++) {
                if (data->ingredients[i].quantity <= 0) {
                    data->ingredients[i].available = false;
                    printf("Chef %d: Ingredient %d is now out of stock\n", employee_id, i);
                }
            }
        }
        
        semaphore_signal(semid, SEM_CHEF);
        semaphore_signal(semid, SEM_MUTEX);
    }
    
    printf("Chef Process %d (employee %d) exiting...\n", getpid(), employee_id);
}
