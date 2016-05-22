#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_lefthours;
static TextLayer *s_time_rightminutes;
static Layer *s_time_squares;

#define SQUARE_WIDTH 10
#define SQUARE_HEIGHT 7
#define SQUARE_PADDING_RIGHT 9
#define SQUARE_PADDING_TOP 6.5

#define LEFT_OFFSET 18
#define TOP_OFFSET 10

GRect square_location(int row, int col) {
    return GRect((row * (SQUARE_WIDTH + SQUARE_PADDING_RIGHT)) + LEFT_OFFSET, (col * (SQUARE_HEIGHT + SQUARE_PADDING_TOP)) + TOP_OFFSET, SQUARE_WIDTH, SQUARE_HEIGHT);
}

void display_layer_update_time(Layer *layer, GContext *ctx) {
    time_t now = time(NULL);
    struct tm * tick_time = localtime(&now);

    graphics_context_set_fill_color(ctx, GColorBlack);

    int minutes = 0;
    int hour = tick_time -> tm_hour;
    
    if (hour > 12) {
        hour = hour - 12;
    }

    if (hour == 0) {
        hour = 12;
    }

    int col, row, height;

    for (col = 0; col < 12; col++) {
        for (row = 0; row < 6; row++) {
            height = col;
            /* Hour mode */
            if (row == 0) {
                if (12 - col > hour) {
                    continue;
                }
            } else {
                if (minutes++ < tick_time -> tm_min) {
                    height = (11 - col);
                } else {
                    continue;
                }
            }

            graphics_draw_rect(ctx, square_location(row, height));
            graphics_fill_rect(ctx, square_location(row, height), 2, GCornersAll);
        }
    }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {    
    layer_mark_dirty(s_time_squares);
}

static void main_window_load(Window *window) {
    // Get information about the Window
    Layer * window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
   
    s_time_lefthours = text_layer_create(GRect(1, -1, bounds.size.w, bounds.size.h));
    s_time_rightminutes = text_layer_create(GRect(0, -1, bounds.size.w, bounds.size.h));
    s_time_squares = layer_create(GRect(0, -1, bounds.size.w, bounds.size.h));   

    text_layer_set_background_color(s_time_lefthours, GColorClear);
    text_layer_set_text_color(s_time_lefthours, GColorBlack);
    text_layer_set_text(s_time_lefthours, "12 \n11 \n10 \n09 \n08 \n07 \n06 \n05 \n04 \n03 \n02 \n01");
    text_layer_set_font(s_time_lefthours, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_time_lefthours, GTextAlignmentLeft);

    text_layer_set_background_color(s_time_rightminutes, GColorClear);
    text_layer_set_text_color(s_time_rightminutes, GColorBlack);
    text_layer_set_text(s_time_rightminutes, "60 \n55 \n50 \n45 \n40 \n35 \n30 \n25 \n20 \n15 \n10 \n05");
    text_layer_set_font(s_time_rightminutes, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_time_rightminutes, GTextAlignmentRight);

    layer_set_update_proc(s_time_squares, display_layer_update_time);

    layer_add_child(window_layer, text_layer_get_layer(s_time_lefthours));
    layer_add_child(window_layer, text_layer_get_layer(s_time_rightminutes));
    layer_add_child(window_get_root_layer(window), s_time_squares);
}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_time_lefthours);
    text_layer_destroy(s_time_rightminutes);
    layer_destroy(s_time_squares);
}

static void init() {
    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
            .unload = main_window_unload
    });

    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);

    // Make sure the time is displayed from the start
    layer_mark_dirty(s_time_squares);

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}