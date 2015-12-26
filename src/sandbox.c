#include <pebble.h>

#define COLOR_FOREGROUND GColorWhite
#define COLOR_BACKGROUND GColorBlack

Window* window;

Layer* background;
TextLayer* hour_text;
TextLayer* minute_text;

// callback function for rendering the background layer
void background_update_callback(Layer *me, GContext* ctx) {
  graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);
  graphics_fill_rect(ctx, GRect(2,14,68,62), 4, GCornersAll);
  graphics_fill_rect(ctx, GRect(74,8,68,68), 4, GCornersAll);

  graphics_context_set_stroke_color(ctx, COLOR_BACKGROUND);
  graphics_draw_line(ctx, GPoint(2,41), GPoint(142,41));
}

// set update the time and date text layers
void display_time(struct tm *tick_time) {
  if (tick_time == NULL) {
		time_t now = time(NULL);
    tick_time = localtime(&now);
  }

  static char hour[]   = "12";
  static char minute[] = "21";

  strftime(hour, sizeof(hour), "%H", tick_time);
  strftime(minute, sizeof(minute), "%S", tick_time);

  text_layer_set_text(hour_text, hour);
  text_layer_set_text(minute_text, minute);
}

// callback function for minute tick events that update the time and date display
void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  display_time(tick_time);
}

/*void update_date_format() {
  int date = 2;
  int month = 38;
  int year = 74;

  GRect rect = layer_get_frame(text_layer_get_layer(date_text));
  rect.origin.x = date;
  layer_set_frame(text_layer_get_layer(date_text), rect);
  rect = layer_get_frame(text_layer_get_layer(month_text));
  rect.origin.x = month;
  layer_set_frame(text_layer_get_layer(month_text), rect);
  rect = layer_get_frame(text_layer_get_layer(year_text));
  rect.origin.x = year;
  layer_set_frame(text_layer_get_layer(year_text), rect);
}*/

// utility function for initializing a text layer
void init_text(TextLayer* textlayer, ResourceId font) {
  text_layer_set_text_alignment(textlayer, GTextAlignmentCenter);
  text_layer_set_text_color(textlayer, COLOR_BACKGROUND);
  text_layer_set_background_color(textlayer, GColorClear);
  text_layer_set_font(textlayer, fonts_load_custom_font(resource_get_handle(font)));
}

// callback function for the app initialization
void handle_init() {
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, COLOR_BACKGROUND);

  background = layer_create(layer_get_frame(window_get_root_layer(window)));
  layer_set_update_proc(background, &background_update_callback);
  layer_add_child(window_get_root_layer(window), background);

  hour_text = text_layer_create(GRect(2, 8 + 4, 68, 68 - 4));
  init_text(hour_text, RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_46);
  minute_text = text_layer_create(GRect(74, 8 + 4, 68, 68 - 4));
  init_text(minute_text, RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_46);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(hour_text));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minute_text));

  display_time(NULL);

  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

void handle_deinit() {
  tick_timer_service_unsubscribe();
  text_layer_destroy(hour_text);
  text_layer_destroy(minute_text);

  // TODO: unload custom fonts

  layer_destroy(background);
  window_destroy(window);
}

// main entry point of this Pebble watchface
int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
