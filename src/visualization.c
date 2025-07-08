#include "visualization.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>

BakeryData* viz_data = NULL;
int viz_semid = -1;

// Adjusted window width to accommodate the new panel
#define WINDOW_WIDTH 1080 // Increased from 780 to fit the third panel
#define WINDOW_HEIGHT 620

void init_visualization(int argc, char** argv, BakeryData* data, int semid) {
    viz_data = data;
    viz_semid = semid;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Bakery Management Dashboard");
    
    glClearColor(BAKERY_YELLOW);
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    
    glutDisplayFunc(display_callback);
    glutTimerFunc(1000/60, timer_callback, 0);
}

void display_callback() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    semaphore_wait(viz_semid, SEM_MUTEX);
    draw_stats_panel();
    draw_customer_panel();
    draw_ingredients_panel(); // New panel for ingredients
    semaphore_signal(viz_semid, SEM_MUTEX);
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void timer_callback(int __attribute__((unused)) value) {
    glutPostRedisplay();
    glutTimerFunc(1000/60, timer_callback, 0);
}

void draw_text(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void draw_customer_panel() {
    // Adjusted customer panel position to make space for the new panel
    glColor4f(0.9f, 0.9f, 0.9f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(520, 20);
    glVertex2f(780, 20);
    glVertex2f(780, 600);
    glVertex2f(520, 600);
    glEnd();
    
    glColor4f(BAKERY_BROWN);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(520, 20);
    glVertex2f(780, 20);
    glVertex2f(780, 600);
    glVertex2f(520, 600);
    glEnd();

    glColor4f(BAKERY_BLACK);
    draw_text(530, 40, "CUSTOMER STATUS");

    int y_pos = 70;
    char buffer[150];
    
    for (int i = 0; i < viz_data->customer_count; i++) {
        if (y_pos > 580) break;
        
        const char* item_name = "Unknown Item";
        for (int j = 0; j < viz_data->item_count; j++) {
            if (viz_data->items[j].id == viz_data->customers[i].requested_item_id) {
                item_name = viz_data->items[j].name;
                break;
            }
        }

        if (viz_data->customers[i].is_served && !viz_data->customers[i].has_complained) {
            glColor4f(0.1f, 0.5f, 0.1f, 1.0f);
        } else if (viz_data->customers[i].is_served && viz_data->customers[i].has_complained) {
            glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
        } else if (viz_data->customers[i].is_frustrated) {
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        } else {
            glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        }

        const char* status = viz_data->customers[i].is_frustrated ? "Frustrated" :
                            (viz_data->customers[i].is_served && viz_data->customers[i].has_complained) ? "Complained" :
                            viz_data->customers[i].is_served ? "Served" : "Waiting";
        
        sprintf(buffer, "Customer %d: %s (%s)", 
                viz_data->customers[i].id, item_name, status);
        draw_text(530, y_pos, buffer);
        y_pos += 20;
    }
    
    glColor4f(BAKERY_BLACK);
}

void draw_ingredients_panel() {
    // New ingredients panel on the right
    glColor4f(0.9f, 0.9f, 0.9f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(800, 20);
    glVertex2f(1060, 20);
    glVertex2f(1060, 600);
    glVertex2f(800, 600);
    glEnd();
    
    glColor4f(BAKERY_BROWN);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(800, 20);
    glVertex2f(1060, 20);
    glVertex2f(1060, 600);
    glVertex2f(800, 600);
    glEnd();

    glColor4f(BAKERY_BLACK);
    draw_text(810, 40, "INGREDIENTS INVENTORY");

    int y_pos = 70;
    char buffer[150];
    
    // List of ingredients with their indices in BakeryData
    const char* ingredient_names[] = {
        "Wheat", "Yeast", "Butter", "Milk", "Sugar and Salt", "Sweet Items", "Cheese and Salami"
    };
    
    // Display each ingredient and its quantity
    for (int i = 0; i < 7; i++) {
        if (y_pos > 580) break;
        
        if (!viz_data->ingredients[i].available || viz_data->ingredients[i].quantity <= 0) {
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // Red for missing
            sprintf(buffer, "%s: Missing", ingredient_names[i]);
        } else {
            glColor4f(0.1f, 0.5f, 0.1f, 1.0f); // Green for available
            sprintf(buffer, "%s: %d", ingredient_names[i], viz_data->ingredients[i].quantity);
        }
        draw_text(810, y_pos, buffer);
        y_pos += 20;
    }

    // Check for teams affected by missing ingredients
    y_pos += 10;
    glColor4f(BAKERY_BLACK);
    draw_text(810, y_pos, "Affected Teams:");
    y_pos += 20;

    bool teams_affected = false;
    // Check which teams are affected based on ingredient dependencies (as defined in chef.c)
    if (!viz_data->ingredients[WHEAT].available || viz_data->ingredients[WHEAT].quantity <= 0 ||
        !viz_data->ingredients[YEAST].available || viz_data->ingredients[YEAST].quantity <= 0) {
        // TEAM_PASTE, TEAM_SWEET_PATISSERIES, TEAM_SAVORY_PATISSERIES depend on Wheat and Yeast
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        draw_text(810, y_pos, "Bread Team (Wheat/Yeast Missing)");
        y_pos += 20;
        draw_text(810, y_pos, "Sweet Patisseries (Wheat/Yeast Missing)");
        y_pos += 20;
        draw_text(810, y_pos, "Savory Patisseries (Wheat/Yeast Missing)");
        y_pos += 20;
        teams_affected = true;
    }
    if (!viz_data->ingredients[CHEESE_SALAMI].available || viz_data->ingredients[CHEESE_SALAMI].quantity <= 0) {
        // TEAM_SANDWICHES and TEAM_SAVORY_PATISSERIES depend on Cheese and Salami
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        draw_text(810, y_pos, "Sandwiches Team (Cheese/Salami Missing)");
        y_pos += 20;
        draw_text(810, y_pos, "Savory Patisseries (Cheese/Salami Missing)");
        y_pos += 20;
        teams_affected = true;
    }
    if (!viz_data->ingredients[SUGAR_SALT].available || viz_data->ingredients[SUGAR_SALT].quantity <= 0 ||
        !viz_data->ingredients[SWEET_ITEMS].available || viz_data->ingredients[SWEET_ITEMS].quantity <= 0) {
        // TEAM_CAKES, TEAM_SWEETS, TEAM_SWEET_PATISSERIES depend on Sugar/Salt and Sweet Items
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        draw_text(810, y_pos, "Cakes Team (Sugar/Sweet Items Missing)");
        y_pos += 20;
        draw_text(810, y_pos, "Sweets Team (Sugar/Sweet Items Missing)");
        y_pos += 20;
        draw_text(810, y_pos, "Sweet Patisseries (Sugar/Sweet Items Missing)");
        y_pos += 20;
        teams_affected = true;
    }

    if (!teams_affected) {
        glColor4f(0.1f, 0.5f, 0.1f, 1.0f);
        draw_text(810, y_pos, "None");
    }

    glColor4f(BAKERY_BLACK);
}

void draw_stats_panel() {
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(20, 20);
    glVertex2f(500, 20);
    glVertex2f(500, 600);
    glVertex2f(20, 600);
    glEnd();
    
    glColor4f(BAKERY_BROWN);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(20, 20);
    glVertex2f(500, 20);
    glVertex2f(500, 600);
    glVertex2f(20, 600);
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); 
    draw_text(30, 40, "BAKERY INVENTORY DASHBOARD");
    
    char buffer[150];
    sprintf(buffer, "TOTAL PROFIT: $%.2f", viz_data->total_profit);
    glColor4f(0.1f, 0.5f, 0.1f, 1.0f);
    draw_text(30, 70, buffer);
    glColor4f(BAKERY_BLACK);

    int y_pos = 100;
    int column1_x = 30;
    int column2_x = 250;

    glColor3f(0.93f, 0.71f, 0.76f);
    draw_text(column1_x, y_pos, "=== BREAD ===");
    y_pos += 20;
    
    int bread_items[3] = {0};
    for (int i = 0; i < viz_data->item_count; i++) {
        if (viz_data->items[i].team_id == TEAM_PASTE && viz_data->items[i].ready) {
            if (strstr(viz_data->items[i].name, "White Bread")) bread_items[0] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Whole Wheat")) bread_items[1] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Burger Buns")) bread_items[2] += viz_data->items[i].quantity;
        }
    }
    
    sprintf(buffer, "White Bread: %d ($%.2f)", bread_items[0], WHITE_BREAD_PRICE);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Whole Wheat: %d ($%.2f)", bread_items[1], WHOLE_WHEAT_BREAD_PRICE);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Burger Buns: %d ($%.2f)", bread_items[2], BURGER_BUNS_PRICE);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;

    glColor3f(0.67f, 0.84f, 0.90f);
    draw_text(column1_x, y_pos, "=== SANDWICHES ===");
    y_pos += 20;
    
    int sandwich_items[3] = {0};
    for (int i = 0; i < viz_data->item_count; i++) {
        if (viz_data->items[i].team_id == TEAM_SANDWICHES && viz_data->items[i].ready) {
            if (strstr(viz_data->items[i].name, "Cheese Sandwich")) sandwich_items[0] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Salami Sandwich")) sandwich_items[1] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Cheese and Salami")) sandwich_items[2] += viz_data->items[i].quantity;
        }
    }
    
    sprintf(buffer, "Cheese: %d ($%.2f)", sandwich_items[0], CHEESE_SANDWICH_PRICE);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Salami: %d ($%.2f)", sandwich_items[1], SALAMI_SANDWICH_PRICE);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Cheese+Salami: %d ($%.2f)", sandwich_items[2], CHEESE_SALAMI_SANDWICH_PRICE);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;

    glColor3f(0.80f, 0.92f, 0.77f);
    draw_text(column1_x, y_pos, "=== CAKES ===");
    y_pos += 20;
    
    int cake_items[3] = {0};
    for (int i = 0; i < viz_data->item_count; i++) {
        if (viz_data->items[i].team_id == TEAM_CAKES && viz_data->items[i].ready) {
            if (strstr(viz_data->items[i].name, "Vanilla")) cake_items[0] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Chocolate")) cake_items[1] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Strawberry")) cake_items[2] += viz_data->items[i].quantity;
        }
    }
    
    sprintf(buffer, "Vanilla: %d ($%.2f)", cake_items[0], VANILLA_CAKE_PRICE);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Chocolate: %d ($%.2f)", cake_items[1], CHOCOLATE_CAKE_PRICE);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Strawberry: %d ($%.2f)", cake_items[2], STRAWBERRY_CAKE_PRICE);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;

    glColor3f(1.00f, 0.84f, 0.72f);
    draw_text(column2_x, 100, "=== SWEETS ===");
    y_pos = 120;
    
    int sweet_items[3] = {0};
    for (int i = 0; i < viz_data->item_count; i++) {
        if (viz_data->items[i].team_id == TEAM_SWEETS && viz_data->items[i].ready) {
            if (strstr(viz_data->items[i].name, "Donuts")) sweet_items[0] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Cupcakes")) sweet_items[1] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Brownies")) sweet_items[2] += viz_data->items[i].quantity;
        }
    }
    
    sprintf(buffer, "Donuts: %d ($%.2f)", sweet_items[0], DONUTS_PRICE);
    draw_text(column2_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Cupcakes: %d ($%.2f)", sweet_items[1], CUPCAKES_PRICE);
    draw_text(column2_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Brownies: %d ($%.2f)", sweet_items[2], BROWNIES_PRICE);
    draw_text(column2_x, y_pos, buffer); y_pos += 20;

    glColor3f(0.87f, 0.63f, 0.87f);
    draw_text(column2_x, y_pos, "=== SWEET PATISSERIES ===");
    y_pos += 20;
    
    int sweet_pastry_items[3] = {0};
    for (int i = 0; i < viz_data->item_count; i++) {
        if (viz_data->items[i].team_id == TEAM_SWEET_PATISSERIES && viz_data->items[i].ready) {
            if (strstr(viz_data->items[i].name, "Cream Rolls")) sweet_pastry_items[0] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Jam Tarts")) sweet_pastry_items[1] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Mini Muffins")) sweet_pastry_items[2] += viz_data->items[i].quantity;
        }
    }
    
    sprintf(buffer, "Cream Rolls: %d ($%.2f)", sweet_pastry_items[0], CREAM_ROLLS_PRICE);
    draw_text(column2_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Jam Tarts: %d ($%.2f)", sweet_pastry_items[1], JAM_TARTS_PRICE);
    draw_text(column2_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Mini Muffins: %d ($%.2f)", sweet_pastry_items[2], MINI_MUFFINS_PRICE);
    draw_text(column2_x, y_pos, buffer); y_pos += 20;

    glColor3f(0.98f, 0.85f, 0.87f);
    draw_text(column2_x, y_pos, "=== SAVORY PATISSERIES ===");
    y_pos += 20;
    
    int savory_pastry_items[3] = {0};
    for (int i = 0; i < viz_data->item_count; i++) {
        if (viz_data->items[i].team_id == TEAM_SAVORY_PATISSERIES && viz_data->items[i].ready) {
            if (strstr(viz_data->items[i].name, "Cheese Puffs")) savory_pastry_items[0] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Mini Pizzas")) savory_pastry_items[1] += viz_data->items[i].quantity;
            else if (strstr(viz_data->items[i].name, "Sausage Rolls")) savory_pastry_items[2] += viz_data->items[i].quantity;
        }
    }
    
    sprintf(buffer, "Cheese Puffs: %d ($%.2f)", savory_pastry_items[0], CHEESE_PUFFS_PRICE);
    draw_text(column2_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Mini Pizzas: %d ($%.2f)", savory_pastry_items[1], MINI_PIZZAS_PRICE);
    draw_text(column2_x, y_pos, buffer); y_pos += 20;
    sprintf(buffer, "Sausage Rolls: %d ($%.2f)", savory_pastry_items[2], SAUSAGE_ROLLS_PRICE);
    draw_text(column2_x, y_pos, buffer); y_pos += 20;

    glColor3f(0.74f, 0.74f, 0.74f);
    draw_text(column1_x, 400, "=== CUSTOMERS ===");
    y_pos = 420;
    
    sprintf(buffer, "Total Customers: %d", viz_data->customer_count);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;
    
    sprintf(buffer, "Active Now: %d", viz_data->customer_count - viz_data->frustrated_customers);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;
    
    sprintf(buffer, "Frustrated: %d", viz_data->frustrated_customers);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;
    
    sprintf(buffer, "Complaints: %d", viz_data->complaining_customers);
    draw_text(column1_x, y_pos, buffer); y_pos += 20;

    glColor3f(0.80f, 0.80f, 0.90f);
    draw_text(column2_x, 400, "=== SALES ===");
    y_pos = 420;
    
    int total_items_sold = 0;
    for (int i = 0; i < viz_data->item_count; i++) {
        if (viz_data->items[i].ready) {
            total_items_sold += viz_data->items[i].quantity;
        }
    }
    sprintf(buffer, "Total Items Sold: %d", total_items_sold);
    draw_text(column2_x, y_pos, buffer); y_pos += 20;
    
    sprintf(buffer, "Daily Target: $1000.00");
    draw_text(column2_x, y_pos, buffer); y_pos += 20;

    glColor4f(BAKERY_BLACK);
}
