#include <pebble.h>
#include <shapes.h>
#include <math.h>
#include <rotate.h>
  
static GPath *s_boxpath;
static GPath *s_number1_path;
static GPath *s_number2_path;
static GPath *s_number3_path;
static GPath *s_number4_path;

static GPath *s_monthday1_path;
static GPath *s_monthday2_path;

static Window *s_main_window;
static Layer *s_canvas_layer;
static Layer *s_box1_layer;
static Layer *s_box2_layer;
static Layer *s_box3_layer;
static Layer *s_box4_layer;
static Layer *s_time_middle_layer;

static GBitmap *s_bitmap;
static BitmapLayer *s_camo_bg_layer;

int8_t currentHour, currentMinute, currentMonthDay;
int8_t previousHour, previousMinute;
static char month_and_weekday_buffer[50];
static char abbrv_month[4], day_of_month[3], full_weekday[10];

int slant_direction;  // 1 is slanted down to right, -1 is slanted up to right

bool digits_changed_during_tick[4] = {0,0,0,0};
int8_t anim_delays[4] = {0,0,0,0};

float ROTATION_ANGLE = (TRIG_MAX_ANGLE*0.073);

#define BOX_DRAW_X_OFFSET (14 + 14*slant_direction)
#define BOX_DRAW_Y_OFFSET (5 + (-5)*slant_direction)
#define DROP_SHADOW_X_OFFSET (slant_direction == 1 ? 2 : 3)
#define DROP_SHADOW_Y_OFFSET (8 - (5*slant_direction))

#define CENTER_SPACE_WIDTH 13 // the horizontal space between the hour and minute digits

#ifdef PBL_ROUND
  #define SCREEN_CENTER_HORIZ 72
#else
  #define SCREEN_CENTER_HORIZ 90
#endif

/*
int drop_shadow_y_offset(){
  if(slant_direction==1){return 3;} else {return 9;}
} 
*/

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
static void set_day_digit1_pathinfo_from_existing(GPathInfo existing1, int target_width, int target_height);
static void set_day_digit2_pathinfo_from_existing(GPathInfo existing2, int target_width, int target_height);

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

static void drop_digit(int which_digit){ // which_digit is one of {0,1,2,3} 
                                        //according to HH:MM => [0][1]:[2][3] in the current time
  GRect start, finish;
  int x_offset = 26;
  int y_offset = 13;
  
  int x = which_digit; // which_digit should be one of: {0,1,2,3}
  
  int anim_duration = 600;
  int initial_delay = 0;
//  int delay_overlap = 60;
  
  // x and y spacing
  
  // for original slant:
  if(slant_direction == 1){
    if(currentHour > 9 || currentHour == 0)  // for 4 digits
    {
      if(currentHour != 0 && currentHour < 20){ // if hour from 10 thru 19, shift numbers 
                              //left slightly to correct for width of number 1
        if(x<2){
          x_offset = x_offset*x - 5;
          y_offset = y_offset*x + 25;
        } else{
          x_offset = x_offset*x + 13 - 5;
          y_offset = y_offset*x + 25 + 4;
        }
      } else {  // hours 20 to 23
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
  } else {
    /// for opposite slant:
    
    if(currentHour > 9 || currentHour == 0){  // for 4 digits
 
      if(currentHour != 0 && currentHour < 20){ // if hour is from 10 thru 19, shift numbers 
                              //left slightly to correct for width of number 1
        
        if(x<2){                              // hours
          x_offset = x_offset*x + 3 - 5;
          y_offset = (slant_direction)*y_offset*x + 72;
        } else{                               // minutes
          x_offset = x_offset*x + 13 + 3 - 5;
          y_offset = (slant_direction)*y_offset*x + 72 - 4;
        }
      } else {  // 4-digits: 20:00 through 23:59
        if(x<2){                             // hours
          x_offset = x_offset*x + 3;
          y_offset = (slant_direction)*y_offset*x + 72;
        } else{                              // minutes
          x_offset = x_offset*x + 13 + 3;
          y_offset = (slant_direction)*y_offset*x + 72 - 4;
        }
      }
    } else{       // for 3 digits
      if(x<2){                               // hours
        x_offset = x_offset*x - 13;
        y_offset = (slant_direction)*y_offset*x + 72 + 6;
      } else{                                // minutes
        x_offset = x_offset*x;
        y_offset = (slant_direction)*y_offset*x + 72 - 4 + 6;
      }
    }
    
    #ifdef PBL_ROUND
      x_offset += 18;
      y_offset += 6;
    #endif
  }
    
  //start = GRect(x_offset, y_offset-168, 50, 68);
  //start = GRect(x_offset, -1*(x+1)*y_offset, 50, 68);
  start = GRect(x_offset, -60, 50, 68);
  finish = GRect(x_offset, y_offset, 50, 68);
  
  //APP_LOG(APP_LOG_LEVEL_INFO,"Start point for number position %d: (%d, %d)",x,finish.origin.x, finish.origin.y);
  
  // configure and schedule animations
  switch (which_digit){
    case 0:
      s_dropin1_animation = property_animation_create_layer_frame(s_box1_layer, &start, &finish);
      animation_set_duration((Animation*)s_dropin1_animation, anim_duration);
      animation_set_delay((Animation*)s_dropin1_animation, initial_delay);    
      animation_set_curve((Animation*)s_dropin1_animation, AnimationCurveEaseOut);
      animation_set_handlers((Animation*)s_dropin1_animation, (AnimationHandlers) {
        .started = dropin1_started_handler
      }, NULL);
      animation_schedule((Animation*)s_dropin1_animation);
      break;
    case 1:
        s_dropin2_animation = property_animation_create_layer_frame(s_box2_layer, &start, &finish);
        animation_set_duration((Animation*)s_dropin2_animation, anim_duration);
        //animation_set_delay((Animation*)s_dropin2_animation, ((digits_changed_during_tick[0])*anim_duration)+(1-digits_changed_during_tick[0])*initial_delay);
        animation_set_delay((Animation*)s_dropin2_animation, initial_delay);
        animation_set_curve((Animation*)s_dropin2_animation, AnimationCurveEaseOut);
        animation_set_handlers((Animation*)s_dropin2_animation, (AnimationHandlers) {
          .started = dropin2_started_handler
        }, NULL);
        animation_schedule((Animation*)s_dropin2_animation);
        break;
      case 2:
        s_dropin3_animation = property_animation_create_layer_frame(s_box3_layer, &start, &finish);
        animation_set_duration((Animation*)s_dropin3_animation, anim_duration);
        //animation_set_delay((Animation*)s_dropin3_animation, ((digits_changed_during_tick[0]+digits_changed_during_tick[1])*anim_duration)/*+initial_delay*/);
        animation_set_delay((Animation*)s_dropin3_animation, initial_delay);
        animation_set_curve((Animation*)s_dropin3_animation, AnimationCurveEaseOut);
        animation_set_handlers((Animation*)s_dropin3_animation, (AnimationHandlers) {
          .started = dropin3_started_handler
        }, NULL);
        animation_schedule((Animation*)s_dropin3_animation);
        break;
      case 3:
        s_dropin4_animation = property_animation_create_layer_frame(s_box4_layer, &start, &finish);
        animation_set_duration((Animation*)s_dropin4_animation, anim_duration);
        //animation_set_delay((Animation*)s_dropin4_animation, ((digits_changed_during_tick[0]+digits_changed_during_tick[1]+digits_changed_during_tick[2])*anim_duration)/*+initial_delay*/);
        animation_set_delay((Animation*)s_dropin4_animation,initial_delay);
        animation_set_curve((Animation*)s_dropin4_animation, AnimationCurveEaseOut);
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
  
  /*
  //for testing
  currentHour = 6;
  currentMinute = 00;
  */
  
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
  
  //use currentMonthDay to set monthday path values
  set_day_digit1_pathinfo_from_existing(time_digit_info_value((int)(currentMonthDay/10)),22,30);
  set_day_digit2_pathinfo_from_existing(time_digit_info_value((int)(currentMonthDay%10)),22,30);

  gpath_destroy(s_monthday1_path);
  gpath_destroy(s_monthday2_path);
  
  s_monthday1_path = gpath_create(ptr_day_digit_1);
  s_monthday2_path = gpath_create(ptr_day_digit_2);
}

static void set_day_digit1_pathinfo_from_existing(GPathInfo existing1, int target_width, int target_height){
  
  float width_scale1 = (float)target_width/(float)STANDARD_DIGIT_WIDTH;
  float height_scale1 = (float)target_height/(float)STANDARD_DIGIT_HEIGHT;
  
  for (uint32_t i = 0; i < existing1.num_points; i++){ // change this to loop through all 14 points (all points in the DAY_DIGIT gpathinfo) and set the trailing points to all be the same...
    DAY_DIGIT_1.points[i].x = existing1.points[i].x * width_scale1;
    DAY_DIGIT_1.points[i].y = existing1.points[i].y * height_scale1;
//    APP_LOG(APP_LOG_LEVEL_INFO,"DAY_DIGIT_1.points[%d] being set to (%d,%d)...result(%d,%d)",(int)i,(int)existing1.points[i].x,(int)existing1.points[i].y,DAY_DIGIT_1.points[i].x,DAY_DIGIT_1.points[i].y);
  }

}

static void set_day_digit2_pathinfo_from_existing(GPathInfo existing2, int target_width, int target_height){
  
  float width_scale2 = (float)target_width/(float)STANDARD_DIGIT_WIDTH;
  float height_scale2 = (float)target_height/(float)STANDARD_DIGIT_HEIGHT;
  
  for (uint32_t i = 0; i < existing2.num_points; i++){ // change this to loop through all 14 points (all points in the DAY_DIGIT gpathinfo) and set the trailing points to all be the same...
    DAY_DIGIT_2.points[i].x = existing2.points[i].x * width_scale2;
    DAY_DIGIT_2.points[i].y = existing2.points[i].y * height_scale2;
    APP_LOG(APP_LOG_LEVEL_INFO,"DAY_DIGIT_2.points[%d] being set to (%d,%d)...result(%d,%d)",(int)i,(int)existing2.points[i].x,(int)existing2.points[i].y,DAY_DIGIT_2.points[i].x,DAY_DIGIT_2.points[i].y);
  }
}


static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  // fill the background with the backround color
 
  /*
  #ifndef PBL_COLOR
  GRect bounds = layer_get_bounds(this_layer);
  graphics_context_set_fill_color(ctx, background_color);
  //graphics_context_set_fill_color(ctx, GColorClear);
  graphics_fill_rect(ctx, bounds, 0, 0);
  #endif
  */
  
  // draw day of month digits
    
  //graphics_context_set_fill_color(ctx, number_color);
  graphics_context_set_fill_color(ctx, GColorBlack);
  
  gpath_rotate_to(s_monthday1_path, ROTATION_ANGLE*slant_direction);
  gpath_move_to(s_monthday1_path,GPoint(23,168-45));
  gpath_draw_filled(ctx,s_monthday1_path);
  
  gpath_rotate_to(s_monthday2_path, ROTATION_ANGLE*slant_direction);
  gpath_move_to(s_monthday2_path,GPoint(47,168-36));
  gpath_draw_filled(ctx,s_monthday2_path);
  
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
 
  update_time();  // update variables containing time data
    
  // animate the changed digits into their places on the canvas
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
  
  // reset all digits to "unchanged" status until next tick event
  digits_changed_during_tick[0] = false;
  digits_changed_during_tick[1] = false;
  digits_changed_during_tick[2] = false;
  digits_changed_during_tick[3] = false;
  
}

static void box1_update_proc(Layer *this_layer, GContext *ctx) {
  
  if(currentHour > 9 || currentHour == 0){
    
    gpath_rotate_to(s_number1_path, ROTATION_ANGLE*slant_direction);
    
    #ifdef PBL_COLOR
      graphics_context_set_fill_color(ctx, path_color);
      gpath_move_to(s_number1_path, GPoint(BOX_DRAW_X_OFFSET + 2,DROP_SHADOW_Y_OFFSET));
      gpath_draw_filled(ctx, s_number1_path);
    #endif
    
    graphics_context_set_fill_color(ctx, number_color);
    gpath_move_to(s_number1_path, GPoint(BOX_DRAW_X_OFFSET,BOX_DRAW_Y_OFFSET));
    
    gpath_draw_filled(ctx, s_number1_path);
    
  } else{
    // handle the case where there are only 3 numbers in the time:
    // ...do nothing
  }
    
}

static void box2_update_proc(Layer *this_layer, GContext *ctx) {
  
  gpath_rotate_to(s_number2_path, ROTATION_ANGLE*slant_direction);
  
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, path_color);
    //gpath_move_to(s_number2_path, GPoint(BOX_DRAW_X_OFFSET + 2,DROP_SHADOW_Y_OFFSET));
    gpath_move_to(s_number2_path, GPoint(BOX_DRAW_X_OFFSET + DROP_SHADOW_X_OFFSET,DROP_SHADOW_Y_OFFSET));
    gpath_draw_filled(ctx, s_number2_path);
  #endif
  
  graphics_context_set_fill_color(ctx, number_color);
  gpath_move_to(s_number2_path, GPoint(BOX_DRAW_X_OFFSET,BOX_DRAW_Y_OFFSET));
  gpath_draw_filled(ctx, s_number2_path);
}

static void box3_update_proc(Layer *this_layer, GContext *ctx) {
  gpath_rotate_to(s_number3_path, ROTATION_ANGLE*slant_direction);
  
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, path_color);
//    gpath_move_to(s_number3_path, GPoint(BOX_DRAW_X_OFFSET + 2,DROP_SHADOW_Y_OFFSET));
    gpath_move_to(s_number3_path, GPoint(BOX_DRAW_X_OFFSET + DROP_SHADOW_X_OFFSET,DROP_SHADOW_Y_OFFSET));
    gpath_draw_filled(ctx, s_number3_path);
  #endif
  
  graphics_context_set_fill_color(ctx, number_color);
  gpath_move_to(s_number3_path, GPoint(BOX_DRAW_X_OFFSET,BOX_DRAW_Y_OFFSET));
  gpath_draw_filled(ctx, s_number3_path);
  
}

static void box4_update_proc(Layer *this_layer, GContext *ctx) {
  
  gpath_rotate_to(s_number4_path, ROTATION_ANGLE*slant_direction);
  
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, path_color);
    //gpath_move_to(s_number4_path, GPoint(BOX_DRAW_X_OFFSET + 2,DROP_SHADOW_Y_OFFSET));
    gpath_move_to(s_number4_path, GPoint(BOX_DRAW_X_OFFSET + DROP_SHADOW_X_OFFSET,DROP_SHADOW_Y_OFFSET));
    gpath_draw_filled(ctx, s_number4_path);
  #endif
  
  graphics_context_set_fill_color(ctx, number_color);
  gpath_move_to(s_number4_path, GPoint(BOX_DRAW_X_OFFSET,BOX_DRAW_Y_OFFSET));
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
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAMO_BG_IMAGE);
  s_camo_bg_layer = bitmap_layer_create(window_bounds);
  bitmap_layer_set_bitmap(s_camo_bg_layer, s_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_camo_bg_layer));
  
  s_canvas_layer = layer_create(window_bounds);
  layer_add_child(bitmap_layer_get_layer(s_camo_bg_layer), s_canvas_layer);
  
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
  
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);//
  
  layer_set_update_proc(s_box1_layer, box1_update_proc);
  layer_set_update_proc(s_box2_layer, box2_update_proc);
  layer_set_update_proc(s_box3_layer, box3_update_proc);
  layer_set_update_proc(s_box4_layer, box4_update_proc);
  
  s_time_middle_layer = layer_create(GRect(-5,60,5,60));
  layer_set_update_proc(s_time_middle_layer, middle_layer_update_proc);
  layer_add_child(window_layer, s_time_middle_layer);
  
  //hide the bitmap layer if on aplite
  /*
  #ifndef PBL_COLOR
    layer_set_hidden(bitmap_layer_get_layer(s_camo_bg_layer), true);
  #endif
  */
}

static void main_window_unload(Window *window){
  
  layer_destroy(s_box1_layer);
  layer_destroy(s_box2_layer);
  layer_destroy(s_box3_layer);
  layer_destroy(s_box4_layer);
  layer_destroy(s_time_middle_layer);
  layer_destroy(s_canvas_layer);

  bitmap_layer_destroy(s_camo_bg_layer);
  gbitmap_destroy(s_bitmap);
}
   
static void init(void){     // set up layers/windows
    
  // green on tan scheme (camo-ish)
  background_color = COLOR_FALLBACK(GColorLimerick, GColorWhite);
  //number_color = GColorDarkGreen;
  number_color = GColorBlack;
  
  // blue on white scheme
  /*background_color = COLOR_FALLBACK(GColorWhite, GColorBlack);
  number_color = COLOR_FALLBACK(GColorVividCerulean, GColorWhite);
  */
  
  // red on blue scheme
  /*background_color = COLOR_FALLBACK(GColorVividCerulean, GColorBlack);
  number_color = COLOR_FALLBACK(GColorRed, GColorWhite);
  */
  
  path_color =  COLOR_FALLBACK(GColorDarkGray, GColorBlack);
  
  slant_direction = 1;
  
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  
  s_boxpath = gpath_create(&BOX_PATH);  // initialize the path info for the time dots
  
  update_time();
   
  digits_changed_during_tick[0] = true;
  digits_changed_during_tick[1] = true;
  digits_changed_during_tick[2] = true;
  digits_changed_during_tick[3] = true;
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  //use currentMonthDay to set monthday path values
  set_day_digit1_pathinfo_from_existing(time_digit_info_value((int)(currentMonthDay/10)),22,30);
  set_day_digit2_pathinfo_from_existing(time_digit_info_value((int)(currentMonthDay%10)),22,30);

  s_monthday1_path = gpath_create(ptr_day_digit_1);
  s_monthday2_path = gpath_create(ptr_day_digit_2);
}

static void deinit(void){   //destroy layers/windows, etc.
  tick_timer_service_unsubscribe();
  gpath_destroy(s_boxpath);
  
  gpath_destroy(s_monthday1_path);
  gpath_destroy(s_monthday2_path);
  
  clean_up_number_gpaths();            // destory all previous gpaths for numbers
  animation_unschedule_all();  // Stop any animation in progress
  window_destroy(s_main_window);  // Destroy main Window
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
