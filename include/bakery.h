#ifndef BAKERY_H
#define BAKERY_H

#include "shared_memory.h"

// In bakery.h

// Item categories and fixed prices
#define WHITE_BREAD_PRICE 3.50
#define WHOLE_WHEAT_BREAD_PRICE 4.00
#define BURGER_BUNS_PRICE 2.50

#define CHEESE_SANDWICH_PRICE 5.00
#define SALAMI_SANDWICH_PRICE 5.50
#define CHEESE_SALAMI_SANDWICH_PRICE 8.00

#define VANILLA_CAKE_PRICE 12.00
#define CHOCOLATE_CAKE_PRICE 14.00
#define STRAWBERRY_CAKE_PRICE 15.00

#define DONUTS_PRICE 2.50
#define CUPCAKES_PRICE 3.00
#define BROWNIES_PRICE 3.50

#define CREAM_ROLLS_PRICE 4.00
#define JAM_TARTS_PRICE 3.50
#define MINI_MUFFINS_PRICE 3.00

#define CHEESE_PUFFS_PRICE 4.50
#define MINI_PIZZAS_PRICE 5.00
#define SAUSAGE_ROLLS_PRICE 5.50

// Team types
#define TEAM_PASTE 0
#define TEAM_CAKES 1
#define TEAM_SANDWICHES 2
#define TEAM_SWEETS 3
#define TEAM_SWEET_PATISSERIES 4
#define TEAM_SAVORY_PATISSERIES 5
#define TEAM_BAKE_CAKES_SWEETS 6
#define TEAM_BAKE_PATISSERIES 7
#define TEAM_BAKE_BREAD 8

// Ingredient indices
#define WHEAT 0
#define YEAST 1
#define BUTTER 2
#define MILK 3
#define SUGAR_SALT 4
#define SWEET_ITEMS 5
#define CHEESE_SALAMI 6

#define TEAM_BAKE_BREAD 6
#define TEAM_BAKE_CAKES_SWEETS 7
#define TEAM_BAKE_PATISSERIES 8
// In bakery.h
typedef enum {
    // Bread (0-2)
    WHITE_BREAD = 0,
    WHOLE_WHEAT_BREAD = 1,
    BURGER_BUNS = 2,
    
    // Sandwiches (3-5)
    CHEESE_SANDWICH = 3,
    SALAMI_SANDWICH = 4,
    CHEESE_SALAMI_SANDWICH = 5,
    
    // Cakes (6-8)
    VANILLA_CAKE = 6,
    CHOCOLATE_CAKE = 7,
    STRAWBERRY_CAKE = 8,
    
    // Sweets (9-11)
    DONUTS = 9,
    CUPCAKES = 10,
    BROWNIES = 11,
    
    // Sweet Patisseries (12-14)
    CREAM_ROLLS = 12,
    JAM_TARTS = 13,
    MINI_MUFFINS = 14,
    
    // Savory Patisseries (15-17)
    CHEESE_PUFFS = 15,
    MINI_PIZZAS = 16,
    SAUSAGE_ROLLS = 17
} BakeryItemID;

// Function prototypes
void initialize_bakery(BakeryData* data);
void run_supply_chain(BakeryData* data, int semid);
void run_chef(BakeryData* data, int semid, int employee_id);
void run_baker(BakeryData* data, int semid, int employee_id);
void run_seller(BakeryData* data, int semid, int employee_id);
void run_customer(BakeryData* data, int semid, int customer_id);
void run_manager(BakeryData* data, int semid);
void initialize_bakery_items(BakeryData* data);

#endif
