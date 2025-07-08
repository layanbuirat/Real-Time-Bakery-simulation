#ifndef VISUALIZATION_H
#define VISUALIZATION_H
#include <math.h>
#include "bakery.h"

#include "shared_memory.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>

// Window dimensions
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700

// Colors
#define BAKERY_YELLOW 0.96f, 0.87f, 0.70f, 1.0f
#define BAKERY_BROWN 0.55f, 0.27f, 0.07f, 1.0f
#define BAKERY_BLACK 0.1f, 0.1f, 0.1f, 1.0f
#define BAKERY_RED 0.8f, 0.2f, 0.2f, 1.0f
#define BAKERY_GREEN 0.4f, 0.7f, 0.4f, 1.0f

// Function prototypes
void init_visualization(int argc, char** argv, BakeryData* data, int semid);
void display_callback();
void timer_callback(int value);
void draw_bakery();
void draw_oven(int x, int y, int width, int height, bool active);
void draw_bread(int x, int y, int size);
void draw_cake(int x, int y, int size);
void draw_counter(int x, int y, int width, int height);
void draw_employee(int x, int y, bool is_chef, bool is_baker, bool is_seller);
void draw_customer(int x, int y, bool is_frustrated, bool has_complained);
void draw_text(float x, float y, const char* text);
void draw_stats_panel();

// Global variables
extern BakeryData* viz_data;
extern int viz_semid;
// Add these prototypes at the top:
void draw_customer_panel(void);
void draw_ingredients_panel(void);

#endif
