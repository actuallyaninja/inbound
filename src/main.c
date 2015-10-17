#include <pebble.h>
#include <shapes.h>
#include <math.h>
#include <rotate.h>
  
static GPath *s_boxpath;
static GPath *s_number1_path;
static GPath *s_number2_path;
static GPath *s_number3_path;
static GPath *s_number4_path;
static Window *s_main_window;
static Layer *s_canvas_layer;
static Layer *s_box1_layer;
static Layer *s_box2_layer;
static Layer *s_box3_layer;
static Layer *s_box4_layer;
static Layer *s_time_middle_layer;

int8_t currentHour, currentMinute, currentMonthDay;
int8_t previousHour, previousMinute;
static char month_and_weekday_buffer[50];
static char abbrv_month[4], day_of_month[3], full_weekday[10];

bool digits_changed_during_tick[4] = {0,0,0,0};
int8_t anim_delays[4] = {0,0,0,0};

float ROTATION_ANGLE = (TRIG_MAX_ANGLE*0.073);

#define BOX_DRAW_X_OFFSET 28

GColor background_color;
GColor number_color;
GColor path_color;

// variable for animation
static PropertyAnimation *s_dropin1_animation;
static PropertyAnimation *s_dropin2_animation;
static PropertyAnimation *s_dropin3_animation;
static PropertyAnimation *s_dropin4_animation;
//static PropertyAnimation *s_middle_layer_animation;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed);

static void dropin1_started_handler(Animation *animation, void *context) {
  //update digit 1 number path value
  gpath_destroy(s_number1_path);
  s_number1_path = gpath_create(time_digit_info((int)floor(currentHour / 10)));
  layer_mark_dirty(s_box1_layer);
}

static void dropin2_started_handler(Animation *animation, void *context) {
  gpath_destroy(s_number2_path);
  s_number2_path = gpath_create(time_digit_info(currentHour % 10));
  layer_mark_dirty(s_box2_layer);
}

static void dropin3_started_handler(Animation *animation, void *context) {
  gpath_destroy(s_number3_path);
  s_number3_path = gpath_create(time_digit_info((int)floor(currentMinute / 10)));
  layer_mark_dirty(s_box3_layer);
}

static void dropin4_started_handler(Animation *animation, void *context) {
  gpath_destroy(s_number4_path);
  s_number4_path = gpath_create(time_digit_info(currentMinute % 10));
  layer_mark_dirty(s_box4_layer);
}

static void drop_digit(int which_digit){
  
  GRect start, finish;
  int x_offset = 26;
  int y_offset = 13;
  
  int x = which_digit; // which_digit should be one of: {0,1,2,3}
  
  int anim_duration = 400;
  int initial_delay = 120;
//  int delay_overlap = 60;
  
  // x and y spacing
  
  if(currentHour > 9 || currentHour == 0)  // for 4 digits
  {
    if(currentHour > 0 && currentHour < 20){ // if hour is between 10 and 12, shift numbers 
                            //left slightly to correct for width of number 1
      if(x<2){
        x_offset = x_offset*x - 5;
        y_offset = y_offset*x + 25;
      } else{
        x_offset = x_offset*x + 13 - 5;
        y_offset = y_offset*x + 25 + 4;
      }
    } else {
      if(x<2){
        x_offset = x_offset*x;
        y_offset = y_offset*x + 25;
      } else{
        x_offset = x_offset*x + 13;
        y_offset = y_offset*x + 25 + 4;
      }
    }
  } else{                                  // for 3 digits
    if(x<2){
      x_offset = x_offset*x - 13;
      y_offset = y_offset*x + 25 - 6;
    } else{
      x_offset = x_offset*x;
      y_offset = y_offset*x + 25 + 4 - 6;
    }
  }
  
  #ifdef PBL_ROUND
    x_offset += 18;
    y_offset += 6;
  #endif
  
  start = GRect(x_offset, y_offset-200, 50, 68);
  finish = GRect(x_offset, y_offset, 50, 68);
  
  // delay for first drop: just the initial delay
  //           second drop: if there are 
  
  switch (which_digit){
    case 0:
      s_dropin1_animation = property_animation_create_layer_frame(s_box1_layer, &start, &finish);
      animation_set_duration((Animation*)s_dropin1_animation, anim_duration);
      animation_set_delay((Animation*)s_dropin1_animation, initial_delay);    
      animation_set_curve((Animation*)s_dropin1_animation, AnimationCurveLinear);
      animation_set_handlers((Animation*)s_dropin1_animation, (AnimationHandlers) {
        .started = dropin1_started_handler
      }, NULL);
      animation_schedule((Animation*)s_dropin1_animation);
      break;
    case 1:
        s_dropin2_animation = property_animation_create_layer_frame(s_box2_layer, &start, &finish);
        animation_set_duration((Animation*)s_dropin2_animation, anim_duration);
        animation_set_delay((Animation*)s_dropin2_animation, ((digits_changed_during_tick[0])*anim_duration)+(1-digits_changed_during_tick[0])*initial_delay);
        animation_set_curve((Animation*)s_dropin2_animation, AnimationCurveLinear);
        animation_set_handlers((Animation*)s_dropin2_animation, (AnimationHandlers) {
          .started = dropin2_started_handler
        }, NULL);
        animation_schedule((Animation*)s_dropin2_animation);
        break;
      case 2:
        s_dropin3_animation = property_animation_create_layer_frame(s_box3_layer, &start, &finish);
        animation_set_duration((Animation*)s_dropin3_animation, anim_duration);
        animation_set_delay((Animation*)s_dropin3_animation, ((digits_changed_during_tick[0]+digits_changed_during_tick[1])*anim_duration)/*+initial_delay*/);
        animation_set_curve((Animation*)s_dropin3_animation, AnimationCurveLinear);
        animation_set_handlers((Animation*)s_dropin3_animation, (AnimationHandlers) {
          .started = dropin3_started_handler
        }, NULL);
        animation_schedule((Animation*)s_dropin3_animation);
        break;
      case 3:
        s_dropin4_animation = property_animation_create_layer_frame(s_box4_layer, &start, &finish);
        animation_set_duration((Animation*)s_dropin4_animation, anim_duration);
        animation_set_delay((Animation*)s_dropin4_animation, ((digits_changed_during_tick[0]+digits_changed_during_tick[1]+digits_changed_during_tick[2])*anim_duration)/*+initial_delay*/);
        animation_set_curve((Animation*)s_dropin4_animation, AnimationCurveLinear);
        animation_set_handlers((Animation*)s_dropin4_animation, (AnimationHandlers) {
          .started = dropin4_started_handler
        }, NULL);
        animation_schedule((Animation*)s_dropin4_animation);
        break;
      default:
        break;
  }
      
}
  
/*
  // set up the start and finish locations for the time divider layer
  GRect middle_start, middle_finish;
  middle_start =  GRect(70,0,5,168);
  middle_finish =  GRect(70,64,5,20);

  s_middle_layer_animation = property_animation_create_layer_frame(s_time_middle_layer, &middle_start, &middle_finish);
  animation_set_duration((Animation*)s_middle_layer_animation, 200);
  animation_set_delay((Animation*)s_middle_layer_animation, (4*200)+50);
  animation_set_curve((Animation*)s_middle_layer_animation, AnimationCurveLinear);
  animation_schedule((Animation*)s_middle_layer_animation);
  */

static void clean_up_number_gpaths(){
  gpath_destroy(s_number1_path);
  gpath_destroy(s_number2_path);
  gpath_destroy(s_number3_path);
  gpath_destroy(s_number4_path);
}

static void update_time() {
  
  previousHour = currentHour;
  previousMinute = currentMinute;
  
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  
  // if 24 hour time not set, make necessary adjustements
  if(!clock_is_24h_style() && tick_time->tm_hour > 12){
    currentHour = tick_time->tm_hour - 12;
  } else {
    currentHour = tick_time->tm_hour;
  }
  currentMinute = tick_time->tm_min;
  
  /**/
  //for testing
  currentHour = 12;
  currentMinute = 00;
  /**/
  
  // after this point, we can tell which digits changed
  if(((int)floor(currentHour / 10)) != ((int)floor(previousHour / 10))){     // hour tens digit
    digits_changed_during_tick[0] = true;
  }
  
  if((currentHour % 10) != (previousHour % 10)){
    digits_changed_during_tick[1] = true;
  }
  
  if(((int)floor(currentMinute / 10)) !=  ((int)floor(previousMinute / 10))){
    digits_changed_during_tick[2] = true;
  }
  
  if((currentMinute % 10) != (previousMinute % 10)){
    digits_changed_during_tick[3] = true;
  }
  
  currentMonthDay = tick_time->tm_mday;
  
  //testing:
  //currentMonthDay = 30;
  
  if(currentMonthDay > 9){
    strftime(month_and_weekday_buffer,sizeof(month_and_weekday_buffer),"%b    %n%A",tick_time);
  } else{
    strftime(month_and_weekday_buffer,sizeof(month_and_weekday_buffer),"%b  %n%A",tick_time);
  }
  
  strftime(abbrv_month,sizeof(abbrv_month),"%b",tick_time);
  snprintf(day_of_month,sizeof(day_of_month),"%d",currentMonthDay);
  strftime(full_weekday,sizeof(full_weekday),"%A",tick_time);
    
}

static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  // fill the background with the backround color
  GRect bounds = layer_get_bounds(this_layer);
  graphics_context_set_fill_color(ctx, background_color);
  graphics_fill_rect(ctx, bounds, 0, 0);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
 
  update_time();
    
  if(digits_changed_during_tick[0]){
    drop_digit(0);  
  }
  
  if(digits_changed_during_tick[1]){
    drop_digit(1);
  }
    
  if(digits_changed_during_tick[2]){
    drop_digit(2);
  }
  
  if(digits_changed_during_tick[3]){
    drop_digit(3);
  }
  
  digits_changed_during_tick[0] = false;
  digits_changed_during_tick[1] = false;
  digits_changed_during_tick[2] = false;
  digits_changed_during_tick[3] = false;
  
}

static void box1_update_proc(Layer *this_layer, GContext *ctx) {
  //gpath_destroy(s_number1_path);
  //s_number1_path = gpath_create(time_digit_info((int)floor(currentHour / 10)));
  
  
  
  if(currentHour > 9 || currentHour == 0){
    #ifdef PBL_COLOR
      //graphics_context_set_stroke_color(ctx, path_color);
      //graphics_context_set_stroke_width(ctx, 3);
      graphics_context_set_fill_color(ctx, path_color);
      gpath_move_to(s_number1_path, GPoint(BOX_DRAW_X_OFFSET + 2,3));
      gpath_draw_filled(ctx, s_number1_path);
    #endif
    
    graphics_context_set_fill_color(ctx, number_color);
    gpath_move_to(s_number1_path, GPoint(BOX_DRAW_X_OFFSET,0));
    gpath_rotate_to(s_number1_path, ROTATION_ANGLE);
    gpath_draw_filled(ctx, s_number1_path);
    
  } else{
    // handle the case where there are only 3 numbers in the time:
    // ...do nothing
  }
    
}

static void box2_update_proc(Layer *this_layer, GContext *ctx) {
  
  #ifdef PBL_COLOR
    //graphics_context_set_stroke_color(ctx, path_color);
    //graphics_context_set_stroke_width(ctx, 3);
    graphics_context_set_fill_color(ctx, path_color);
    gpath_move_to(s_number2_path, GPoint(BOX_DRAW_X_OFFSET + 2,3));
    gpath_draw_filled(ctx, s_number2_path);
  #endif
  
  graphics_context_set_fill_color(ctx, number_color);
  gpath_move_to(s_number2_path, GPoint(BOX_DRAW_X_OFFSET,0));
  gpath_rotate_to(s_number2_path, ROTATION_ANGLE);
  gpath_draw_filled(ctx, s_number2_path);
}

static void box3_update_proc(Layer *this_layer, GContext *ctx) {
  
  #ifdef PBL_COLOR
    //graphics_context_set_stroke_color(ctx, path_color);
    //graphics_context_set_stroke_width(ctx, 3);
    graphics_context_set_fill_color(ctx, path_color);
    gpath_move_to(s_number3_path, GPoint(BOX_DRAW_X_OFFSET + 2,3));
    gpath_draw_filled(ctx, s_number3_path);
  #endif
  
  graphics_context_set_fill_color(ctx, number_color);
  gpath_move_to(s_number3_path, GPoint(BOX_DRAW_X_OFFSET,0));
  gpath_rotate_to(s_number3_path, ROTATION_ANGLE);
  gpath_draw_filled(ctx, s_number3_path);
  
}

static void box4_update_proc(Layer *this_layer, GContext *ctx) {
  
  #ifdef PBL_COLOR
    //graphics_context_set_stroke_color(ctx, path_color);
    //graphics_context_set_stroke_width(ctx, 3);
    graphics_context_set_fill_color(ctx, path_color);
    gpath_move_to(s_number4_path, GPoint(BOX_DRAW_X_OFFSET + 2,3));
    gpath_draw_filled(ctx, s_number4_path);
  #endif
  
  graphics_context_set_fill_color(ctx, number_color);
  gpath_move_to(s_number4_path, GPoint(BOX_DRAW_X_OFFSET,0));
  gpath_rotate_to(s_number4_path, ROTATION_ANGLE);
  gpath_draw_filled(ctx, s_number4_path);

}

static void middle_layer_update_proc(Layer *this_layer, GContext *ctx){
  GRect layer_bounds = layer_get_bounds(this_layer);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_bounds,0,0);
}


static void main_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  // Create Layers
  s_canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, s_canvas_layer);
  
  GRect left_number_box = GRect(-50, -68, 50, 68);
  GRect right_number_box = GRect(194, 68, 50, 68);
  
  s_box1_layer = layer_create(left_number_box); 
  layer_add_child(s_canvas_layer, s_box1_layer);
  
  s_box2_layer = layer_create(left_number_box); 
  layer_add_child(s_canvas_layer, s_box2_layer);
  
  s_box3_layer = layer_create(right_number_box); 
  layer_add_child(s_canvas_layer, s_box3_layer);
  
  s_box4_layer = layer_create(right_number_box);
  layer_add_child(s_canvas_layer, s_box4_layer);
  
  window_set_background_color(s_main_window, GColorBlack);
  
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  layer_set_update_proc(s_box1_layer, box1_update_proc);
  layer_set_update_proc(s_box2_layer, box2_update_proc);
  layer_set_update_proc(s_box3_layer, box3_update_proc);
  layer_set_update_proc(s_box4_layer, box4_update_proc);
  
  s_time_middle_layer = layer_create(GRect(-5,60,5,60));
  layer_set_update_proc(s_time_middle_layer, middle_layer_update_proc);
  layer_add_child(window_layer, s_time_middle_layer);
  
}

static void main_window_unload(Window *window){
  
  layer_destroy(s_box1_layer);
  layer_destroy(s_box2_layer);
  layer_destroy(s_box3_layer);
  layer_destroy(s_box4_layer);
  layer_destroy(s_time_middle_layer);
  layer_destroy(s_canvas_layer);
}
   
static void init(void){     // set up layers/windows
  //background_color = COLOR_FALLBACK(GColorBrass, GColorBlack);
  background_color = COLOR_FALLBACK(GColorLimerick, GColorBlack);
  number_color = COLOR_FALLBACK(GColorDarkGreen, GColorWhite);
  path_color =  GColorBlack;
  
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  
  s_boxpath = gpath_create(&BOX_PATH);  // initialize the path info for the time dots
  
  update_time();
  previousHour = currentHour;
  previousMinute = currentMinute;
   
  digits_changed_during_tick[0] = true;
  digits_changed_during_tick[1] = true;
  digits_changed_during_tick[2] = true;
  digits_changed_during_tick[3] = true;
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
}

static void deinit(void){   //destroy layers/windows, etc.
  tick_timer_service_unsubscribe();
  gpath_destroy(s_boxpath);
  clean_up_number_gpaths();            // destory all previous gpaths for numbers
  animation_unschedule_all();  // Stop any animation in progress
  window_destroy(s_main_window);  // Destroy main Window
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
