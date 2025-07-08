#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdbool.h>

#define MAX_CATEGORIES 18
#define MAX_TEAMS 10
#define MAX_EMPLOYEES 50
#define MAX_CUSTOMERS 100
#define MAX_ITEMS 100

// Semaphore indices
#define SEM_MUTEX 0
#define SEM_SUPPLY 1
#define SEM_CHEF 2
#define SEM_BAKER 3
#define SEM_SELLER 4
#define SEM_CUSTOMER 5
#define NUM_SEMAPHORES 6

typedef struct {
    int quantity;
    bool available;
} Ingredient;

typedef struct {
    int id;
    char name[50];
    int quantity;
    float price;
    int team_id;
    bool ready;
} BakeryItem;

typedef struct {
    int id;
    char name[50];
    int employee_count;
    int current_workload;
    bool active;
    int items_prepared; 
} Team;

typedef struct {
    int id;
    char name[50];
    int team_id;
    bool busy;
} Employee;

typedef struct {
    int id;
    int requested_item_id;
    bool is_served;
    bool is_frustrated;
    bool has_complained;
    time_t arrival_time;
} Customer;

typedef struct {
    // Shared data
    int bakery_time;  // in minutes
    float total_profit;
    int frustrated_customers;
    int complaining_customers;
    int missing_item_customers;
    
    // Configuration
    int max_frustrated_customers;
    int max_complaining_customers;
    int max_missing_item_customers;
    float profit_threshold;
    int simulation_time_limit;
    
    // Resources
    Ingredient ingredients[7]; // wheat, yeast, butter, milk, sugar_salt, sweet_items, cheese_salami
    BakeryItem items[MAX_ITEMS];
    int item_count;
    
    // Teams and employees
    Team teams[MAX_TEAMS];
    int team_count;
    Employee employees[MAX_EMPLOYEES];
    int employee_count;
    
    // Customers
    Customer customers[MAX_CUSTOMERS];
    int customer_count;
    
    // Simulation control
    bool simulation_running;
} BakeryData;

// Function prototypes
int create_shared_memory();
BakeryData* attach_shared_memory(int shmid);
void detach_shared_memory(BakeryData* data);
void destroy_shared_memory(int shmid);

int create_semaphores();
void semaphore_wait(int semid, int sem_num);
void semaphore_signal(int semid, int sem_num);
void destroy_semaphores(int semid);

#endif
