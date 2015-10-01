#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_hours_text_layer;
static Layer *s_canvas_layer;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_canvas_layer);
}

static char* get_hours_text() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char s_hours_buffer[] = "00";

  if (clock_is_24h_style()) {
    strftime(s_hours_buffer, sizeof("00"), "%H", tick_time);
  } else {
    strftime(s_hours_buffer, sizeof("00"), "%H", tick_time);
  }
  
  return s_hours_buffer;
}

static void update_shadow(GContext *ctx) {
  uint8_t SHADOW_LENGTH = 72;
  char* text_hours = get_hours_text();
  
#ifdef PBL_PLATFORM_BASALT
  GFont text_font = fonts_load_custom_font(FONT_KEY_LECO_42_NUMBERS);
#else
  GFont text_font = fonts_load_custom_font(FONT_KEY_BITHAM_42_BOLD);
#endif
  
#ifdef PBL_COLOR
  graphics_context_set_text_color(ctx, GColorJazzberryJam);
#else
  graphics_context_set_text_color(ctx, GColorWhite);
#endif
  
  GRect text_frame;
  GRect bounds = layer_get_frame(window_get_root_layer(s_main_window));
  
  for (int i = 0; i < SHADOW_LENGTH; i++) {
    // 144 x 168
    text_frame = GRect(72 - i, 148 - i, bounds.size.w, 48);
    graphics_draw_text(ctx, text_hours, text_font, text_frame, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL); 
  }
}

static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  update_shadow(ctx);
}

static void window_main_load(Window *window) {
  Layer *root_layer = window_get_root_layer(s_main_window);
  GRect window_bounds = layer_get_bounds(root_layer);
  s_canvas_layer = layer_create(window_bounds);
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(root_layer, s_canvas_layer);
}

static void window_main_unload(Window *window) {
  text_layer_destroy(s_hours_text_layer);
  layer_destroy(s_canvas_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_main_load,
    .unload = window_main_unload
  });

  bool animated = true;
  window_stack_push(s_main_window, animated);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
