#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_utc_layer;
static TextLayer *s_date_layer;
static TextLayer *s_day_layer;

static void main_window_load(Window *window) {
	//APP_LOG(APP_LOG, "Enter main_window_load");
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
	
	// Create date TextLayer
	s_date_layer = text_layer_create(GRect(0, 100, bounds.size.w, 30));
	text_layer_set_background_color(s_date_layer, GColorBlack);
	text_layer_set_text_color(s_date_layer, GColorWhite);
	text_layer_set_text(s_date_layer, "September 00");
	
	// Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 44, bounds.size.w, 58));
	text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
	
	// Create utc TextLayer
  s_utc_layer = text_layer_create(GRect(0, 134, bounds.size.w, 34));
	text_layer_set_background_color(s_utc_layer, GColorBlack);
  text_layer_set_text_color(s_utc_layer, GColorWhite);
  text_layer_set_text(s_utc_layer, "00:00");

	// Create day TextLayer
  s_day_layer = text_layer_create(GRect(0, 8, bounds.size.w, 34));
	text_layer_set_background_color(s_day_layer, GColorBlack);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_text(s_day_layer, "Wednesday");
	
  // Improve the layout to be more like a watchface
  //text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
	
  text_layer_set_font(s_utc_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_utc_layer, GTextAlignmentCenter);

  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
	
  // Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_utc_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_day_layer));
}

static void main_window_unload(Window *window) {
    // Destroy TextLayer
    text_layer_destroy(s_time_layer);
		text_layer_destroy(s_date_layer);
		text_layer_destroy(s_utc_layer);
		text_layer_destroy(s_day_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
	struct tm *utc_time = gmtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
	static char datebuffer[] = "September 00";
	static char utcbuffer[] = "00:00 - 00/00";
	static char daybuffer[] = "Wednesday";
	
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
		strftime(utcbuffer, sizeof("00:00 - 00/00"), "%H:%M - %d/%m", utc_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
		strftime(utcbuffer, sizeof("00:00 - 00/00"), "%I:%M - %d/%m", utc_time);
  }

	
	// Display Day
	strftime(daybuffer, sizeof("Wednesday"), "%A", tick_time);
	// Display Month / Day
	strftime(datebuffer, sizeof("September 00"), "%B %d", tick_time);
	
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
	text_layer_set_text(s_date_layer, datebuffer);
	text_layer_set_text(s_utc_layer, utcbuffer);
	text_layer_set_text(s_day_layer, daybuffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


static void init() {
	
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

	window_set_background_color(s_main_window, GColorBlack);
	
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
	
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
	// update the time
	update_time();
}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
		tick_timer_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}