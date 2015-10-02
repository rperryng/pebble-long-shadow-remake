#include <pebble.h>
  
#define CENTER GPoint(6, 50)

static Window *s_main_window;
static Layer *s_canvas_layer;
static const uint8_t SHADOW_LENGTH = 94;

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

static int32_t get_minutes_angle() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  return TRIG_MAX_ANGLE * tick_time->tm_min / 60;
}

static void update_shadow(GContext *ctx) {
  char* text_hours = get_hours_text();
  GFont text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LECO_REGULAR_54));
#ifdef PBL_COLOR
  graphics_context_set_text_color(ctx, GColorJazzberryJam);
#else
  graphics_context_set_text_color(ctx, GColorWhite);
#endif
  
  GRect text_frame;
  GRect bounds = layer_get_frame(window_get_root_layer(s_main_window));
  int32_t minutes_angle = get_minutes_angle();
  
  for (int i = 0; i < SHADOW_LENGTH; i++) {
    uint8_t intermediate_length = SHADOW_LENGTH - i;
    uint8_t shadow_x = (sin_lookup(minutes_angle) * intermediate_length / TRIG_MAX_RATIO) + CENTER.x;
    uint8_t shadow_y = (-cos_lookup(minutes_angle) * intermediate_length / TRIG_MAX_RATIO) + CENTER.y;
    text_frame = GRect(shadow_x, shadow_y, bounds.size.w, 60);
    
    graphics_draw_text(ctx, text_hours, text_font, text_frame, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL); 
  }
  
#ifdef PBL_COLOR
  graphics_context_set_text_color(ctx, GColorWhite);
#endif
  graphics_draw_text(ctx, text_hours, text_font, text_frame, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL); 
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
