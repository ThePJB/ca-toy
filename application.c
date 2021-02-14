#include "application.h"
#include "SDL.h"
#include "predator_prey.h"
#include "traffic_jam.h"
#include "wolfram.h"
#include "menu.h"
#include "util.h"

const rule rules[] = {
    (rule) {
        .draw = rule_predator_prey_draw,
        .init = rule_predator_prey_init,
        .update = rule_predator_prey_update,
        .menu_succ = rule_predator_prey_menu_succ,
        .menu_str = rule_predator_prey_menu_str,
        .reset = rule_predator_prey_reset,
        .name = "predator prey",
    },
    (rule) {
        .draw = rule_traffic_jam_draw,
        .init = rule_traffic_jam_init,
        .update = rule_traffic_jam_update,
        .menu_succ = rule_traffic_jam_menu_succ,
        .menu_str = rule_traffic_jam_menu_str,
        .reset = rule_traffic_jam_reset,
        .name = "bml traffic",
    },
    (rule) {
        .draw = rule_wolfram_draw,
        .init = rule_wolfram_init,
        .update = rule_wolfram_update,
        .menu_succ = rule_wolfram_succ,
        .menu_str = rule_wolfram_menu,
        .reset = rule_wolfram_reset_grid,
        .name = "elementary CA point",
    },
    (rule) {
        .draw = rule_wolfram_draw,
        .init = rule_wolfram_init_random,
        .update = rule_wolfram_update,
        .menu_succ = rule_wolfram_succ,
        .menu_str = rule_wolfram_menu,
        .reset = rule_wolfram_reset_grid_random,
        .name = "elementary CA random",
    },
};

bool app_menu_str(void *state, int index, char *buf) {
    application *app = (application*)state;

    if (index == 0) {
        sprintf(buf, "selected CA: %s", app->current_rule.name);
        return true;
    } else if (index == 1) {
        sprintf(buf, "updates per frame %d", app->updates_per_frame);
        return true;
    } else if (index == 2) {
        sprintf(buf, "frame cap %d", app->gc.frame_cap);
        return true;
    } else if (index == 3) {
        sprintf(buf, "scale %d", app->scale);
        return true;
    } else if (index == 4) {
        sprintf(buf, "quit");
        return true;
    } else {
        return false;
    }
}

void app_menu_succ(void *state, int index, bool up) {
    application *app = (application*)state;

    if (index == 0) {
        if (up && app->current_rule_idx < len(rules) + 1) {
            app->current_rule_idx++;
        } else if (!up && app->current_rule_idx > 0) {
            app->current_rule_idx--;
        } else {
            return;
        }
        app->current_rule = rules[app->current_rule_idx];
        free(app->simulation_state);
        app->simulation_state = app->current_rule.init(app->w, app->h);
        menu_clear(&app->ca_menu);
        menu_populate(&app->ca_menu, &app->gc, app->simulation_state, app->current_rule.menu_str);
    } else if (index == 1) {
        if (up) {
            app->updates_per_frame++;
        } else {
            if (app->updates_per_frame == 1) return;
            app->updates_per_frame--;
        }
    } else if (index == 2) {
        if (up) {
            app->gc.frame_cap++;
        } else {
            if (app->gc.frame_cap == 1) return;
            app->gc.frame_cap--;
        }
    } else if (index == 3) {
        if (up) {
            app->scale++;
        } else {
            if (app->scale == 1) return;
            app->scale--;
        }
        // todo remove multiple sources of truth
        app->xres = app->scale * app->w;
        app->yres = app->scale * app->h;
        app->gc.xres = app->xres;
        app->gc.yres = app->yres;
        SDL_SetWindowSize(app->gc.window, app->xres, app->yres);
    } else if (index == 4) {
        if (up) {
            app->keep_going = false;
        }
    }
}

void application_handle_input(application *app) {
    


    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {

        // figure out what functions and stuff to call for the currently active menu
        // ideally this would just be encapsulated nicely... app menu would be the same as a ca menu
        bool (*current_menu_str_function)(void *state, int index, char *buf);
        void (*current_menu_succ_function)(void *state, int index, bool up);
        void *current_menu_state_ptr;

        if (app->current_menu == &app->app_menu) {
            current_menu_state_ptr = app;
            current_menu_str_function = app_menu_str;
            current_menu_succ_function = app_menu_succ;
        } else {
            current_menu_state_ptr = app->simulation_state;
            current_menu_str_function = app->current_rule.menu_str;
            current_menu_succ_function = app->current_rule.menu_succ;
        }


        if (e.type == SDL_QUIT) {
            app->keep_going = false;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            SDL_Keycode sym = e.key.keysym.sym;

            bool up = sym == SDLK_k || sym == SDLK_w || sym == SDLK_UP;
            bool left = sym == SDLK_h || sym == SDLK_a || sym == SDLK_LEFT;
            bool right = sym == SDLK_l || sym == SDLK_d || sym == SDLK_RIGHT;
            bool down = sym == SDLK_j || sym == SDLK_s || sym == SDLK_DOWN;

            if (sym == SDLK_ESCAPE) {
                if (app->current_menu == &app->app_menu) {
                    app->current_menu = NULL;
                } else {
                    app->current_menu = &app->app_menu;
                }
            } else if (sym == SDLK_n) {
                app->current_rule.update(app->simulation_state);
            } else if (sym == SDLK_m) {
                if (app->current_menu == &app->ca_menu) {
                    app->current_menu = NULL;
                } else {
                    app->current_menu = &app->ca_menu;
                }
            } else if (sym == SDLK_SPACE) {
                app->paused = !app->paused;
            } else if (sym == SDLK_r) {
                app->current_rule.reset(app->simulation_state, app->w, app->h);
            } else if (left && app->current_menu) {
                menu_edit(app->current_menu, &app->gc, false, current_menu_state_ptr, current_menu_succ_function, current_menu_str_function);
            } else if (right && app->current_menu) {
                menu_edit(app->current_menu, &app->gc, true, current_menu_state_ptr, current_menu_succ_function, current_menu_str_function);
            } else if (down && app->current_menu) {
                menu_scroll(app->current_menu, false);
            } else if (up && app->current_menu) {
                menu_scroll(app->current_menu, true);
            }
        }
    }
}

void application_draw(application *app) {
    gef_clear(&app->gc);

    app->current_rule.draw(&app->gc, app->simulation_state, app->scale);
    if (app->current_menu) {
        menu_draw(&app->gc, app->current_menu, 0, 0);
    }

    gef_present(&app->gc);
}

void application_step(application *app) {
    if (!app->paused) {
        for (int i = 0; i < app->updates_per_frame; i++) {
            app->current_rule.update(app->simulation_state);
        }
    }
}

application application_init(int w, int h, int scale) {

    int xres = w*scale;
    int yres = h*scale;

    srand(1234567);

    const int start_rule_idx = 0;

    application app = (application) {
        .xres = xres,
        .yres = yres,
        .w = w,
        .h = h,
        .scale = scale,
        .gc = gef_init("ca-toy", xres, yres, 60),
        .keep_going = true,
        .paused = true,
        .current_rule = rules[start_rule_idx],
        .current_rule_idx = start_rule_idx,
        .updates_per_frame = 1,
        .ca_menu = {
            .current_selection = 0,
            .entries = {0},
            .font = gef_load_font("Hack-Regular.ttf", 24),
            .num_items = 0,
        },
        .app_menu = {
            .current_selection = 0,
            .entries = {0},
            .font = gef_load_font("Hack-Regular.ttf", 24),
            .num_items = 0,
        },
    };


    app.simulation_state = app.current_rule.init(w, h);

    menu_populate(&app.ca_menu, &app.gc, app.simulation_state, app.current_rule.menu_str);
    menu_populate(&app.app_menu, &app.gc, &app, app_menu_str);

    return app;
}