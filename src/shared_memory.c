#include "shared_memory.h"
#include "bakery.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Create shared memory segment
int create_shared_memory() {
    int shmid = shmget(IPC_PRIVATE, sizeof(BakeryData), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

// Attach shared memory segment
BakeryData* attach_shared_memory(int shmid) {
    BakeryData* data = (BakeryData*)shmat(shmid, NULL, 0);
    if (data == (BakeryData*)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    return data;
}

// Detach shared memory segment
void detach_shared_memory(BakeryData* data) {
    if (shmdt(data) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}

// Destroy shared memory segment
void destroy_shared_memory(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(EXIT_FAILURE);
    }
}

void initialize_bakery(BakeryData* data) {
    memset(data, 0, sizeof(BakeryData));

    // Initialize ingredients with random quantities
    srand(time(NULL));
    data->ingredients[WHEAT] = (Ingredient){(rand() % 51) + 50, true}; // 50-100
    data->ingredients[YEAST] = (Ingredient){(rand() % 31) + 20, true}; // 20-50
    data->ingredients[BUTTER] = (Ingredient){(rand() % 21) + 20, true}; // 20-40
    data->ingredients[MILK] = (Ingredient){(rand() % 31) + 30, true}; // 30-60
    data->ingredients[SUGAR_SALT] = (Ingredient){(rand() % 41) + 40, true}; // 40-80
    data->ingredients[SWEET_ITEMS] = (Ingredient){(rand() % 21) + 20, true}; // 20-40
    data->ingredients[CHEESE_SALAMI] = (Ingredient){(rand() % 31) + 20, true}; // 20-50

    /* ========= BREAD ITEMS (TEAM_PASTE) ========= */
    // White Bread (ID 0)
    data->items[WHITE_BREAD] = (BakeryItem){
        .id = WHITE_BREAD,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_PASTE,
        .ready = true,
        .price = WHITE_BREAD_PRICE
    };
    strcpy(data->items[WHITE_BREAD].name, "White Bread");

    // Whole Wheat Bread (ID 1)
    data->items[WHOLE_WHEAT_BREAD] = (BakeryItem){
        .id = WHOLE_WHEAT_BREAD,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_PASTE,
        .ready = true,
        .price = WHOLE_WHEAT_BREAD_PRICE
    };
    strcpy(data->items[WHOLE_WHEAT_BREAD].name, "Whole Wheat Bread");

    // Burger Buns (ID 2)
    data->items[BURGER_BUNS] = (BakeryItem){
        .id = BURGER_BUNS,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_PASTE,
        .ready = true,
        .price = BURGER_BUNS_PRICE
    };
    strcpy(data->items[BURGER_BUNS].name, "Burger Buns");

    /* ========= SANDWICH ITEMS (TEAM_SANDWICHES) ========= */
    // Cheese Sandwich (ID 3)
    data->items[CHEESE_SANDWICH] = (BakeryItem){
        .id = CHEESE_SANDWICH,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SANDWICHES,
        .ready = true,
        .price = CHEESE_SANDWICH_PRICE
    };
    strcpy(data->items[CHEESE_SANDWICH].name, "Cheese Sandwich");

    // Salami Sandwich (ID 4)
    data->items[SALAMI_SANDWICH] = (BakeryItem){
        .id = SALAMI_SANDWICH,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SANDWICHES,
        .ready = true,
        .price = SALAMI_SANDWICH_PRICE
    };
    strcpy(data->items[SALAMI_SANDWICH].name, "Salami Sandwich");

    // Cheese and Salami Sandwich (ID 5)
    data->items[CHEESE_SALAMI_SANDWICH] = (BakeryItem){
        .id = CHEESE_SALAMI_SANDWICH,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SANDWICHES,
        .ready = true,
        .price = CHEESE_SALAMI_SANDWICH_PRICE
    };
    strcpy(data->items[CHEESE_SALAMI_SANDWICH].name, "Cheese and Salami Sandwich");

    /* ========= CAKE ITEMS (TEAM_CAKES) ========= */
    // Vanilla Cake (ID 6)
    data->items[VANILLA_CAKE] = (BakeryItem){
        .id = VANILLA_CAKE,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_CAKES,
        .ready = true,
        .price = VANILLA_CAKE_PRICE
    };
    strcpy(data->items[VANILLA_CAKE].name, "Vanilla Cake");

    // Chocolate Cake (ID 7)
    data->items[CHOCOLATE_CAKE] = (BakeryItem){
        .id = CHOCOLATE_CAKE,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_CAKES,
        .ready = true,
        .price = CHOCOLATE_CAKE_PRICE
    };
    strcpy(data->items[CHOCOLATE_CAKE].name, "Chocolate Cake");

    // Strawberry Cake 
    data->items[STRAWBERRY_CAKE] = (BakeryItem){
        .id = STRAWBERRY_CAKE,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_CAKES,
        .ready = true,
        .price = STRAWBERRY_CAKE_PRICE
    };
    strcpy(data->items[STRAWBERRY_CAKE].name, "Strawberry Cake");

    /* ========= SWEET ITEMS (TEAM_SWEETS) ========= */
    // Donuts (ID 9)
    data->items[DONUTS] = (BakeryItem){
        .id = DONUTS,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SWEETS,
        .ready = true,
        .price = DONUTS_PRICE
    };
    strcpy(data->items[DONUTS].name, "Donuts");

    // Cupcakes (ID 10)
    data->items[CUPCAKES] = (BakeryItem){
        .id = CUPCAKES,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SWEETS,
        .ready = true,
        .price = CUPCAKES_PRICE
    };
    strcpy(data->items[CUPCAKES].name, "Cupcakes");

    // Brownies (ID 11)
    data->items[BROWNIES] = (BakeryItem){
        .id = BROWNIES,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SWEETS,
        .ready = true,
        .price = BROWNIES_PRICE
    };
    strcpy(data->items[BROWNIES].name, "Brownies");

    /* ========= SWEET PATISSERIES (TEAM_SWEET_PATISSERIES) ========= */
    // Cream Rolls (ID 12)
    data->items[CREAM_ROLLS] = (BakeryItem){
        .id = CREAM_ROLLS,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SWEET_PATISSERIES,
        .ready = true,
        .price = CREAM_ROLLS_PRICE
    };
    strcpy(data->items[CREAM_ROLLS].name, "Cream Rolls");

    // Jam Tarts (ID 13)
    data->items[JAM_TARTS] = (BakeryItem){
        .id = JAM_TARTS,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SWEET_PATISSERIES,
        .ready = true,
        .price = JAM_TARTS_PRICE
    };
    strcpy(data->items[JAM_TARTS].name, "Jam Tarts");

    // Mini Muffins (ID 14)
    data->items[MINI_MUFFINS] = (BakeryItem){
        .id = MINI_MUFFINS,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SWEET_PATISSERIES,
        .ready = true,
        .price = MINI_MUFFINS_PRICE
    };
    strcpy(data->items[MINI_MUFFINS].name, "Mini Muffins");

    /* ========= SAVORY PATISSERIES (TEAM_SAVORY_PATISSERIES) ========= */
    // Cheese Puffs (ID 15)
    data->items[CHEESE_PUFFS] = (BakeryItem){
        .id = CHEESE_PUFFS,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SAVORY_PATISSERIES,
        .ready = true,
        .price = CHEESE_PUFFS_PRICE
    };
    strcpy(data->items[CHEESE_PUFFS].name, "Cheese Puffs");

    // Mini Pizzas (ID 16)
    data->items[MINI_PIZZAS] = (BakeryItem){
        .id = MINI_PIZZAS,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SAVORY_PATISSERIES,
        .ready = true,
        .price = MINI_PIZZAS_PRICE
    };
    strcpy(data->items[MINI_PIZZAS].name, "Mini Pizzas");

    // Sausage Rolls (ID 17)
    data->items[SAUSAGE_ROLLS] = (BakeryItem){
        .id = SAUSAGE_ROLLS,
        .quantity = (rand() % 2) + 1,
        .team_id = TEAM_SAVORY_PATISSERIES,
        .ready = true,
        .price = SAUSAGE_ROLLS_PRICE
    };
    strcpy(data->items[SAUSAGE_ROLLS].name, "Sausage Rolls");

    data->item_count = 18;
}

// Create semaphores
int create_semaphores() {
    int semid = semget(IPC_PRIVATE, NUM_SEMAPHORES, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }
    
    // Initialize all semaphores to 1
    unsigned short init_values[NUM_SEMAPHORES];
    for (int i = 0; i < NUM_SEMAPHORES; i++) {
        init_values[i] = 1;
    }
    
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    arg.array = init_values;
    
    if (semctl(semid, 0, SETALL, arg) == -1) {
        perror("semctl SETALL failed");
        exit(EXIT_FAILURE);
    }
    
    return semid;
}

// Semaphore wait operation
void semaphore_wait(int semid, int sem_num) {
    struct sembuf op = {sem_num, -1, 0};
    if (semop(semid, &op, 1) == -1) {
        perror("semop wait failed");
        exit(EXIT_FAILURE);
    }
}

// Semaphore signal operation
void semaphore_signal(int semid, int sem_num) {
    struct sembuf op = {sem_num, 1, 0};
    if (semop(semid, &op, 1) == -1) {
        perror("semop signal failed");
        exit(EXIT_FAILURE);
    }
}

// Destroy semaphores
void destroy_semaphores(int semid) {
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl IPC_RMID failed");
        exit(EXIT_FAILURE);
    }
}
