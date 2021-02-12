#include "application.h"
#include "gef.h"
#include "strings.h"

// time for gridx gridy scale

int main(int argc, char** argv) {
    int w = 640;
    int h = 480;
    int scale = 1;

    for (int i = 0; i < argc; i++) {
        if (strings_equal(argv[i], "--width")) {
            i++;
            if (i < argc && strings_is_dec_int(argv[i])) {
                w = atoi(argv[i]);
            } else {
                printf("need an integer argument for width\n");
                return 1;
            }
        } else if (strings_equal(argv[i], "--height")) {
            i++;
            if (i < argc && strings_is_dec_int(argv[i])) {
                h = atoi(argv[i]);
            } else {
                printf("need an integer argument for height\n");
                return 1;
            }
        } else if (strings_equal(argv[i], "--scale")) {
            i++;
            if (i < argc && strings_is_dec_int(argv[i])) {
                scale = atoi(argv[i]);
            } else {
                printf("need an integer argument for scale\n");
                return 1;
            }
        } else if (strings_equal(argv[i], "-h")) {
            printf("supports --width --height --scale\n");
            return 10;
        }
    }

    application app = application_init(w, h, scale);

    while (app.keep_going) {
        gef_start_frame(&app.gc);
        application_handle_input(&app);
        application_step(&app);
        application_draw(&app);
        gef_end_frame(&app.gc);
    }

    return 0;
}