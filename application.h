#pragma once

#include <stdbool.h>

#include "menu.h"
#include "gef.h"

typedef struct {
    void *(*init)(int xres, int yres);                     // initialize the context for the rule from scratch
    void (*update)(void *);                                // update one step
    void (*draw)(gef_context *, void *, int scale);        // duh
    void (*reset)(void *state, int xres, int yres);        // reset grid, keeping settings that have been changed
    bool (*menu_str)(void *state, int index, char *buf);   // display i'th menu entry
    void (*menu_succ)(void *state, int index, bool up);    // modify i'th menu entry
    char *name;
} rule;

typedef struct {
    int xres;
    int yres;
    int w;
    int h;
    int scale;
    
    gef_context gc;
    bool keep_going;
    bool paused;
    void *simulation_state;
    rule current_rule;
    int current_rule_idx;
    menu *current_menu;
    menu app_menu;
    menu ca_menu;
    int updates_per_frame;
} application;

void application_handle_input(application *app);
void application_draw(application *app);
void application_step(application *app);

application application_init(int w, int h, int scale);