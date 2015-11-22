#include <pebble.h>
#include <shapes.h>
#include <math.h>
#include <extramath.h>
#include <positioning.h>

//#define KEY_SLANT_DIRECTION 0
#define KEY_SLANT_DIR_NUM 0
#define KEY_BG_IMAGE 1
#define KEY_BG_COLOR 2

//static GPath *s_boxpath;
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
//static Layer *s_time_middle_layer;

static GBitmap *s_camo_bitmap;
static BitmapLayer *s_camo_bg_layer;

int8_t currentHour, currentMinute, currentMonthDay;
int8_t previousHour, previousMinute;
static char month_and_weekday_buffer[50];
static char abbrv_month[4], day_of_month[3], full_weekday[10];

int slant_direction;  // 1 is slanted down to right, -1 is slanted up to right

bool digits_changed_during_tick[4] = {0,0,0,0};
int8_t anim_delays[4] = {0,0,0,0};

int32_t bg_image_selection;

#define ROTATION_ANGLE (TRIG_MAX_ANGLE*0.073)

#define BOX_DRAW_X_OFFSET (14 + 14*slant_direction - (slant_direction == -1 ? 2 : 0))
#define BOX_DRAW_Y_OFFSET (5 + (-5)*slant_direction)
#define DROP_SHADOW_X_OFFSET (slant_direction == 1 ? 3 : 5)
#define DROP_SHADOW_Y_OFFSET (8 - (5*slant_direction) + (slant_direction == 1 ? 2 : 0))
#define MD_DROP_SHADOW_X_OFFSET (slant_direction == 1 ? 0 : 1)
#define MD_DROP_SHADOW_Y_OFFSET (slant_direction == 1 ? 4 : 2)


#define CENTER_SPACE_WIDTH 13 // the horizontal space between the hour and minute digits

#ifdef PBL_ROUND
  #define SCREEN_CENTER_HORIZ 90
  #define MONTHDAY_X_OFFSET_1 (slant_direction == 1 ? 72 : 60 )
  #define MONTHDAY_Y_OFFSET_1 (slant_direction == 1 ? 124 : 145 )
  #define MONTHDAY_X_OFFSET_2 (slant_direction == 1 ? 97 : 85 )  
  #define MONTHDAY_Y_OFFSET_2 (slant_direction == 1 ? 136 : 134 )
#else
  #define SCREEN_CENTER_HORIZ 72
  #define MONTHDAY_X_OFFSET_1 (slant_direction == 1 ? 25 : 63 )
  #define MONTHDAY_Y_OFFSET_1 (slant_direction == 1 ? (168-62) : (168-38) )
  #define MONTHDAY_X_OFFSET_2 (slant_direction == 1 ? 47 : 86 )  
  #define MONTHDAY_Y_OFFSET_2 (slant_direction == 1 ? (168-52) : (168-49) )
#endif

GColor background_color;
GColor number_color;
GColor path_color;
GColor shadow_color;

// variable for animation
static PropertyAnimation *s_dropin1_animation;
static PropertyAnimation *s_dropin2_animation;
static PropertyAnimation *s_dropin3_animation;
static PropertyAnimation *s_dropin4_animation;
static PropertyAnimation *s_pull1_animation;
static PropertyAnimation *s_pull2_animation;
static PropertyAnimation *s_pull3_animation;
static PropertyAnimation *s_pull4_animation;
//static PropertyAnimation *s_middle_layer_animation;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void set_day_digit1_pathinfo_from_existing(GPathInfo existing1, int target_width, int target_height);
static void set_day_digit2_pathinfo_from_existing(GPathInfo existing2, int target_width, int target_height);

static void drop_digit(int which_digit);
//

static void pull1_finished_handler(Animation *animation, bool finished, void *context){
  #ifdef PBL_SDK_2
    property_animation_destroy((PropertyAnimation*)animation);
  #endif
  // start the dropin animation
  drop_digit(0);
}

static void pull2_finished_handler(Animation *animation, bool finished, void *context){
  #ifdef PBL_SDK_2
    property_animation_destroy((PropertyAnimation*)animation);
  #endif
  // start the dropin animation
  drop_digit(1);
}
static void pull3_finished_handler(Animation *animation, bool finished, void *context){
  #ifdef PBL_SDK_2
    property_animation_destroy((PropertyAnimation*)animation);
  #endif
  // start the dropin animation
  drop_digit(2);
}

static void pull4_finished_handler(Animation *animation, bool finished, void *context){
  #ifdef PBL_SDK_2
    property_animation_destroy((PropertyAnimation*)animation);
  #endif
  // start the dropin animation
  drop_digit(3);
}

static void pull_digit(int which_digit){ // which_digit is one of {0,1,2,3} 
                                        //according to HH:MM => [0][1]:[2][3] in the current time
  
  //APP_LOG(APP_LOG_LEVEL_INFO,"pull_digit(%d) started.", which_digit);
  
  struct GRect * pull_finish;
  GRect p_finish = GRect(72,-73,55,68);
  
  int x = which_digit; // which_digit should be one of: {0,1,2,3}
  
  int anim_duration = 600;
  int initial_delay = 0;
    
  // move the shapes off the screen to the top right if slanted right, top left if slanted left
  if(slant_direction == 1){
    p_finish = GRect(x*30+90, -73, 55, 68);
  }else{
    p_finish = GRect(x*30-80, -73, 55, 68);
  }
  pull_finish = &p_finish;
  
  // configure and schedule animations
  switch (which_digit){
    case 0:        
      s_pull1_animation = property_animation_create_layer_frame(s_box1_layer, NULL, pull_finish);
      animation_set_duration((Animation*)s_pull1_animation, anim_duration);
      animation_set_delay((Animation*)s_pull1_animation, initial_delay);    
      animation_set_curve((Animation*)s_pull1_animation, AnimationCurveEaseIn);
      animation_set_handlers((Animation*)s_pull1_animation, (AnimationHandlers) {
        .stopped = pull1_finished_handler
      }, NULL);
      animation_schedule((Animation*)s_pull1_animation);
      break;
    case 1:
      s_pull2_animation = property_animation_create_layer_frame(s_box2_layer, NULL, pull_finish);
      animation_set_duration((Animation*)s_pull2_animation, anim_duration);
      animation_set_delay((Animation*)s_pull2_animation, initial_delay);
      animation_set_curve((Animation*)s_pull2_animation, AnimationCurveEaseIn);
      animation_set_handlers((Animation*)s_pull2_animation, (AnimationHandlers) {
            .stopped = pull2_finished_handler
          }, NULL);
      animation_schedule((Animation*)s_pull2_animation);
      break;
    case 2:
      s_pull3_animation = property_animation_create_layer_frame(s_box3_layer, NULL, pull_finish);
      animation_set_duration((Animation*)s_pull3_animation, anim_duration);
      animation_set_delay((Animation*)s_pull3_animation, initial_delay);
      animation_set_curve((Animation*)s_pull3_animation, AnimationCurveEaseIn);
      animation_set_handlers((Animation*)s_pull3_animation, (AnimationHandlers) {
            .stopped = pull3_finished_handler
          }, NULL);
      animation_schedule((Animation*)s_pull3_animation);
      break;
    case 3:
       s_pull4_animation = property_animation_create_layer_frame(s_box4_layer, NULL, pull_finish);    
      animation_set_duration((Animation*)s_pull4_animation, anim_duration);      
      animation_set_delay((Animation*)s_pull4_animation,initial_delay);
      animation_set_curve((Animation*)s_pull4_animation, AnimationCurveEaseIn);
      animation_set_handlers((Animation*)s_pull4_animation, (AnimationHandlers) {
        .stopped = pull4_finished_handler
      }, NULL);
      animation_schedule((Animation*)s_pull4_animation);
      break;
    default:
      break;
  }
  
}

static void dropin1_started_handler(Animation *animation, void *context) {
  //layer_mark_dirty(s_box1_layer);
}

static void dropin2_started_handler(Animation *animation, void *context) {
//  APP_LOG(APP_LOG_LEVEL_INFO,"Starting dropin2 animation");
  
  //gpath_destroy(s_number2_path);
  //s_number2_path = gpath_create(time_digit_info(currentHour % 10));
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"digit 2 - new gpath created - marking layer dirty next");
  //layer_mark_dirty(s_box2_layer);
}

static void dropin3_started_handler(Animation *animation, void *context) {
  //APP_LOG(APP_LOG_LEVEL_INFO,"Starting dropin3 animation");
  
  //gpath_destroy(s_number3_path);
  //s_number3_path = gpath_create(time_digit_info((int)floor(currentMinute / 10)));
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"digit 3 - new gpath created - marking layer dirty next");
  //layer_mark_dirty(s_box3_layer);
}

static void dropin4_started_handler(Animation *animation, void *context) {
  //APP_LOG(APP_LOG_LEVEL_INFO,"Starting dropin4 animation");
  
  //gpath_destroy(s_number4_path);
  //s_number4_path = gpath_create(time_digit_info(currentMinute % 10));
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"digit 4 - new gpath created - marking layer dirty next");
  //layer_mark_dirty(s_box4_layer);
}

static void dropin_stopped_handler(Animation *animation, bool finished, void *context){
  #ifdef PBL_SDK_2
    property_animation_destroy((PropertyAnimation*)animation);
  #endif
}
  
static void drop_digit(int which_digit){ // which_digit is one of {0,1,2,3} 
                                        //according to HH:MM => [0][1]:[2][3] in the current time
  //APP_LOG(APP_LOG_LEVEL_INFO,"drop digit started. Heap bytes used | free: %d | %d", (int)heap_bytes_used,(int)heap_bytes_free());
  
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
        
        //new method for calculating point positions:
        GPoint test = {0,0};
        test = number_point_setup(GPoint(0,26), NUMBER_SPACING, CENTER_SPACE_WIDTH, x, M_PI / 7); // this configuration worked for both round and rect :)
        x_offset = test.x;
        y_offset = test.y;
        //APP_LOG(APP_LOG_LEVEL_DEBUG,"Finished number point setup for digit %d", x);
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
    
  //start the shapes off the screen to the bottom left if slanted left, bottom right if slanted right
  if(slant_direction == 1){
    start = GRect(-55, 185, 55, 68);
  }else{
    start = GRect(185, 185, 55, 68);
  }
  
  //start = GRect(x_offset, -60, 50, 68);
  finish = GRect(x_offset, y_offset, 55, 68);
   
  //APP_LOG(APP_LOG_LEVEL_INFO,"Origin point for number layer position %d: (%d, %d)",x,finish.origin.x, finish.origin.y);
  
  // configure and schedule animations
  switch (which_digit){
    case 0:
      gpath_destroy(s_number1_path);
      s_number1_path = gpath_create(time_digit_info((int)floor(currentHour / 10)));
      
      s_dropin1_animation = property_animation_create_layer_frame(s_box1_layer, &start, &finish);
      animation_set_duration((Animation*)s_dropin1_animation, anim_duration);
      animation_set_delay((Animation*)s_dropin1_animation, initial_delay);    
      animation_set_curve((Animation*)s_dropin1_animation, AnimationCurveEaseOut);
      animation_set_handlers((Animation*)s_dropin1_animation, (AnimationHandlers) {
        .started = dropin1_started_handler,
        .stopped = dropin_stopped_handler
      }, NULL);
      animation_schedule((Animation*)s_dropin1_animation);
      break;
    case 1:
        gpath_destroy(s_number2_path);
        s_number2_path = gpath_create(time_digit_info(currentHour % 10));
    
        s_dropin2_animation = property_animation_create_layer_frame(s_box2_layer, &start, &finish);
        animation_set_duration((Animation*)s_dropin2_animation, anim_duration);
        animation_set_delay((Animation*)s_dropin2_animation, initial_delay);
        animation_set_curve((Animation*)s_dropin2_animation, AnimationCurveEaseOut);
        animation_set_handlers((Animation*)s_dropin2_animation, (AnimationHandlers) {
          .started = dropin2_started_handler,
          .stopped = dropin_stopped_handler      
        }, NULL);
        animation_schedule((Animation*)s_dropin2_animation);
        break;
      case 2:
        gpath_destroy(s_number3_path);
        s_number3_path = gpath_create(time_digit_info((int)floor(currentMinute / 10)));
    
        s_dropin3_animation = property_animation_create_layer_frame(s_box3_layer, &start, &finish);
        animation_set_duration((Animation*)s_dropin3_animation, anim_duration);
        animation_set_delay((Animation*)s_dropin3_animation, initial_delay);
        animation_set_curve((Animation*)s_dropin3_animation, AnimationCurveEaseOut);
        animation_set_handlers((Animation*)s_dropin3_animation, (AnimationHandlers) {
          .started = dropin3_started_handler,
          .stopped = dropin_stopped_handler
        }, NULL);
        animation_schedule((Animation*)s_dropin3_animation);
        break;
      case 3:
        gpath_destroy(s_number4_path);
        s_number4_path = gpath_create(time_digit_info(currentMinute % 10));    
    
        s_dropin4_animation = property_animation_create_layer_frame(s_box4_layer, &start, &finish);
        animation_set_duration((Animation*)s_dropin4_animation, anim_duration);
        animation_set_delay((Animation*)s_dropin4_animation,initial_delay);
        animation_set_curve((Animation*)s_dropin4_animation, AnimationCurveEaseOut);
        animation_set_handlers((Animation*)s_dropin4_animation, (AnimationHandlers) {
          .started = dropin4_started_handler,
          .stopped = dropin_stopped_handler
        }, NULL);
        animation_schedule((Animation*)s_dropin4_animation);
        break;
      default:
        break;
  }
  
}
  
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
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"Time updated from %d:%d to %d:%d", previousHour, previousMinute, currentHour, currentMinute);
  /*
  //for testing
  currentHour = 1;
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
  //currentMonthDay = 13;
  
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
  

  float width_scale1 = (float)target_width/(float)STD_BOXY_DIGIT_WIDTH;
  float height_scale1 = (float)target_height/(float)STD_BOXY_DIGIT_HEIGHT;
  
  int last_point_index = 0;
  
  //for (uint32_t i = 0; i < existing1.num_points; i++){ // change this to loop through all 14 points (all points in the DAY_DIGIT gpathinfo) and set the trailing points to all be the same...
  for (uint32_t i = 0; i < 14; i++){  
    if(i < existing1.num_points){
      DAY_DIGIT_1.points[i].x = existing1.points[i].x * width_scale1;
      DAY_DIGIT_1.points[i].y = existing1.points[i].y * height_scale1;
      last_point_index = i;
    } else{
      DAY_DIGIT_1.points[i].x = existing1.points[last_point_index].x * width_scale1;
      DAY_DIGIT_1.points[i].y = existing1.points[last_point_index].y * height_scale1;
    }
//    APP_LOG(APP_LOG_LEVEL_INFO,"DAY_DIGIT_1.points[%d] being set to (%d,%d)...result(%d,%d)",(int)i,(int)existing1.points[i].x,(int)existing1.points[i].y,DAY_DIGIT_1.points[i].x,DAY_DIGIT_1.points[i].y);
  }

}

static void set_day_digit2_pathinfo_from_existing(GPathInfo existing2, int target_width, int target_height){
  
  //float width_scale2 = (float)target_width/(float)STANDARD_DIGIT_WIDTH;
  //float height_scale2 = (float)target_height/(float)STANDARD_DIGIT_HEIGHT;
  float width_scale2 = (float)target_width/(float)STD_BOXY_DIGIT_WIDTH;
  float height_scale2 = (float)target_height/(float)STD_BOXY_DIGIT_HEIGHT;
  
  int last_point_index2 = 0;
  
  //for (uint32_t i = 0; i < existing2.num_points; i++){ // change this to loop through all 14 points (all points in the DAY_DIGIT gpathinfo) and set the trailing points to all be the same...
  for (uint32_t i = 0; i < 14; i++){    
    if(i < existing2.num_points){
      DAY_DIGIT_2.points[i].x = existing2.points[i].x * width_scale2;
      DAY_DIGIT_2.points[i].y = existing2.points[i].y * height_scale2;
      last_point_index2 = i;
  //  APP_LOG(APP_LOG_LEVEL_INFO,"DAY_DIGIT_2.points[%d] being set to (%d,%d)...result(%d,%d)",(int)i,(int)existing2.points[i].x,(int)existing2.points[i].y,DAY_DIGIT_2.points[i].x,DAY_DIGIT_2.points[i].y);
    } else {
      DAY_DIGIT_2.points[i].x = existing2.points[last_point_index2].x * width_scale2;
      DAY_DIGIT_2.points[i].y = existing2.points[last_point_index2].y * height_scale2;
    }
  }
}


static void canvas_update_proc(Layer *this_layer, GContext *ctx) {

  
  gpath_rotate_to(s_monthday1_path, ROTATION_ANGLE*slant_direction);
  gpath_rotate_to(s_monthday2_path, ROTATION_ANGLE*slant_direction);
  // draw drop shadows
  graphics_context_set_fill_color(ctx, shadow_color);
  
  //#ifdef PBL_COLOR
  // draw drop shadow 1
  gpath_move_to(s_monthday1_path,GPoint(MONTHDAY_X_OFFSET_1 + DROP_SHADOW_X_OFFSET, MONTHDAY_Y_OFFSET_1 + MD_DROP_SHADOW_Y_OFFSET));  
  gpath_draw_filled(ctx,s_monthday1_path);
  // draw drop shadow 2
  gpath_move_to(s_monthday2_path,GPoint(MONTHDAY_X_OFFSET_2 + DROP_SHADOW_X_OFFSET, MONTHDAY_Y_OFFSET_2 + MD_DROP_SHADOW_Y_OFFSET));  
  gpath_draw_filled(ctx,s_monthday2_path);
  //#endif
  
  // draw day of month digits
    
  
  //graphics_context_set_fill_color(ctx, number_color);
  graphics_context_set_fill_color(ctx, number_color);
  
  // move foreground number 1 into position
  gpath_move_to(s_monthday1_path,GPoint(MONTHDAY_X_OFFSET_1, MONTHDAY_Y_OFFSET_1));  
  // move foreground number 2 into position
  gpath_move_to(s_monthday2_path,GPoint(MONTHDAY_X_OFFSET_2, MONTHDAY_Y_OFFSET_2));  
  
  // draw the foreground numbers
  gpath_draw_filled(ctx,s_monthday1_path);
  gpath_draw_filled(ctx,s_monthday2_path);

//  APP_LOG(APP_LOG_LEVEL_INFO,"Heap bytes used | free: %d | %d", (int)heap_bytes_used,(int)heap_bytes_free());
    
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"Finishing canvas_update_proc");
  
}

static void push_all_digits(){
    
  // animate the changed digits into place
  if(digits_changed_during_tick[0]){
    pull_digit(0);
  }
  if(digits_changed_during_tick[1]){
    pull_digit(1);
  }
  if(digits_changed_during_tick[2]){
    pull_digit(2);
  }
  if(digits_changed_during_tick[3]){
    pull_digit(3);
  }  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
 
  update_time();  // update variables containing time data
  
  push_all_digits();
  
  // reset all digits to "unchanged" status until next tick event
  digits_changed_during_tick[0] = false;
  digits_changed_during_tick[1] = false;
  digits_changed_during_tick[2] = false;
  digits_changed_during_tick[3] = false;
  
  
}

static void box1_update_proc(Layer *this_layer, GContext *ctx) {
  
  if(currentHour > 9 || currentHour == 0){
    
    gpath_rotate_to(s_number1_path, ROTATION_ANGLE*slant_direction);
    
    // drop shadows for color pebbles
    
    //#ifdef PBL_COLOR
      graphics_context_set_fill_color(ctx, shadow_color);
      gpath_move_to(s_number1_path, GPoint(BOX_DRAW_X_OFFSET + DROP_SHADOW_X_OFFSET,DROP_SHADOW_Y_OFFSET));
      gpath_draw_filled(ctx, s_number1_path);
    //#endif
    
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
  
  // drop shadows for color pebbles
  //#ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, shadow_color);
    gpath_move_to(s_number2_path, GPoint(BOX_DRAW_X_OFFSET + DROP_SHADOW_X_OFFSET,DROP_SHADOW_Y_OFFSET));
    gpath_draw_filled(ctx, s_number2_path);
  //#endif
  
  graphics_context_set_fill_color(ctx, number_color);
  gpath_move_to(s_number2_path, GPoint(BOX_DRAW_X_OFFSET,BOX_DRAW_Y_OFFSET));

  gpath_draw_filled(ctx, s_number2_path); 
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"finishing box2_update_proc");
  
}

static void box3_update_proc(Layer *this_layer, GContext *ctx) {
  
  gpath_rotate_to(s_number3_path, ROTATION_ANGLE*slant_direction);
  
  // drop shadows for color pebbles
  //#ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, shadow_color);
    gpath_move_to(s_number3_path, GPoint(BOX_DRAW_X_OFFSET + DROP_SHADOW_X_OFFSET,DROP_SHADOW_Y_OFFSET));
    gpath_draw_filled(ctx, s_number3_path);
  //#endif
    
  graphics_context_set_fill_color(ctx, number_color);
  gpath_move_to(s_number3_path, GPoint(BOX_DRAW_X_OFFSET,BOX_DRAW_Y_OFFSET));
  
  gpath_draw_filled(ctx, s_number3_path);
  
}

static void box4_update_proc(Layer *this_layer, GContext *ctx) {
  
  gpath_rotate_to(s_number4_path, ROTATION_ANGLE*slant_direction);
  
  // drop shadows for color pebbles
  //#ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, shadow_color);
    gpath_move_to(s_number4_path, GPoint(BOX_DRAW_X_OFFSET + DROP_SHADOW_X_OFFSET,DROP_SHADOW_Y_OFFSET));
    gpath_draw_filled(ctx, s_number4_path);
  //#endif
  
  graphics_context_set_fill_color(ctx, number_color);
  gpath_move_to(s_number4_path, GPoint(BOX_DRAW_X_OFFSET,BOX_DRAW_Y_OFFSET));
  
  gpath_draw_filled(ctx, s_number4_path);
 
}

/*
static void middle_layer_update_proc(Layer *this_layer, GContext *ctx){
  GRect layer_bounds = layer_get_bounds(this_layer);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_bounds,0,0);
}
*/

#ifdef PBL_COLOR
static void set_background_color(int color) {
  GColor background_color = GColorFromHEX(color);
  bitmap_layer_set_background_color(s_camo_bg_layer, background_color);
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Setting bg color to: %d",color);
}
#endif


static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  
  Tuple *slant_dir_num_t = dict_find(iter, KEY_SLANT_DIR_NUM);
  Tuple *bg_image_t = dict_find(iter, KEY_BG_IMAGE);
  Tuple *background_color_t = dict_find(iter, KEY_BG_COLOR);

  if (background_color_t) {
    int background_color = background_color_t->value->int32;

    persist_write_int(KEY_BG_COLOR, background_color);

    // set the background color for color watches only
    #ifdef PBL_COLOR
    set_background_color(background_color);
    #endif
  }
  
  int old_slant_direction = slant_direction;

  //set the slant_direction based on input from config page
  int slant_direction_num = atoi(slant_dir_num_t->value->cstring);
  
  switch (slant_direction_num){
    case 1: slant_direction = 1; break;
    case 2: slant_direction = -1; break;
    default: slant_direction = -1; break;
  }
  
  const int32_t const_slant_direction_num = (int32_t)slant_direction_num;
  //persist_write_int(KEY_SLANT_DIR_NUM, slant_direction_num);
  persist_write_int(KEY_SLANT_DIR_NUM, const_slant_direction_num);
  
  //set value of value from the dictionary and translate to an integer
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"from config page: slant_dir_num_t = %d",atoi(slant_dir_num_t->value->cstring));
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"slant_direction_num value in persistent storage: %d", (int)persist_read_int(KEY_SLANT_DIR_NUM));
  
  // reset digit layout if the direction is different after config data is returned
  if (slant_direction != old_slant_direction){
    digits_changed_during_tick[0] = true;
    digits_changed_during_tick[1] = true;
    digits_changed_during_tick[2] = true;
    digits_changed_during_tick[3] = true;
  
    push_all_digits();
    layer_mark_dirty(s_canvas_layer);
  
    digits_changed_during_tick[0] = false;
    digits_changed_during_tick[1] = false;
    digits_changed_during_tick[2] = false;
    digits_changed_during_tick[3] = false;
  }
  
  // set the value of big image selection from the dictionary
  const int32_t new_bg_image_selection = (int32_t)atoi(bg_image_t->value->cstring);
  persist_write_int(KEY_BG_IMAGE, new_bg_image_selection);
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"new_bg_image_selection: %d", (int)new_bg_image_selection);
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"bg_image_selection value stored in persistent storage: %d", (int)persist_read_int(KEY_BG_IMAGE));
  
  if(bg_image_selection != new_bg_image_selection){ // don't do anything if the bg selection didn't change.
    
    bg_image_selection = new_bg_image_selection;
        
    // reset the gbitmap value and redraw the layer
    gbitmap_destroy(s_camo_bitmap);
    switch (bg_image_selection){
      case 1: s_camo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAMO_BG_IMAGE); break;
      case 2: s_camo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAMO_RED); break;
      case 3: s_camo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAMO_BLUE); break;
      case 4: s_camo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PSYCH_CAMO); break;
      default: s_camo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAMO_BG_IMAGE); break;
    }
      
    bitmap_layer_set_bitmap(s_camo_bg_layer, s_camo_bitmap);
    layer_mark_dirty(bitmap_layer_get_layer(s_camo_bg_layer));
    
  }
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"inbox message recd. - set slant direction to %d", slant_direction);
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"inbox message recd. - slant_direction_t->value->int8 %d", slant_direction_t->value->int8);
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"inbox message recd. - set slant dir num tuple value: %s", slant_dir_num_t->value->cstring);
  //APP_LOG(APP_LOG_LEVEL_DEBUG,"inbox message recd. - bg image tuple value: %s", bg_image_t->value->cstring);
  
  
  //persist_write_int(KEY_SLANT_DIR_NUM, (int)slant_dir_num_t->value->int32);
  persist_write_int(KEY_BG_IMAGE, bg_image_selection);

  
}

static void main_window_load(Window *window){
  
  // read in persistent config values and set variables
  
  // set slant direction from persistent storage
  if(persist_exists(KEY_SLANT_DIR_NUM)){
    switch(persist_read_int(KEY_SLANT_DIR_NUM)){
      case 1: slant_direction = 1; break;
      case 2: slant_direction = -1; break;
      default: slant_direction = -1; break;
    }
  }
  else {
    slant_direction = -1;
  }
  
  //set backgroud image choice from persistent storage
  if(persist_exists(KEY_BG_IMAGE)){
    bg_image_selection = persist_read_int(KEY_BG_IMAGE);
  } else {
    bg_image_selection = 1;
  }  // default to 1 if no persistent storage exists yet
  
  switch (bg_image_selection){
    case 1: s_camo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAMO_BG_IMAGE); break;
    case 2: s_camo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAMO_RED); break;
    case 3: s_camo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAMO_BLUE); break;
    case 4: s_camo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PSYCH_CAMO); break;
    default: s_camo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CAMO_BG_IMAGE); break;
  }
  
  // Create Layers
  
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  s_camo_bg_layer = bitmap_layer_create(window_bounds);
  bitmap_layer_set_bitmap(s_camo_bg_layer, s_camo_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_camo_bg_layer));
  
  #ifdef PBL_COLOR
  bitmap_layer_set_compositing_mode(s_camo_bg_layer, GCompOpSet);
  if (persist_read_int(KEY_BG_COLOR)) {
    int background_color = persist_read_int(KEY_BG_COLOR);
    set_background_color(background_color);
  } else {
    set_background_color(0);
  }
  #endif
  
  s_canvas_layer = layer_create(window_bounds);
  //layer_add_child(bitmap_layer_get_layer(s_camo_bg_layer), s_canvas_layer);
  layer_add_child(window_layer, s_canvas_layer);
  
  GRect start_number_box = GRect(72,-100,55,68);
 
  s_box1_layer = layer_create(start_number_box); 
  //layer_add_child(s_canvas_layer, s_box1_layer);
  layer_add_child(window_layer, s_box1_layer);
  
  s_box2_layer = layer_create(start_number_box); 
  //layer_add_child(s_canvas_layer, s_box2_layer);
  layer_add_child(window_layer, s_box2_layer);
  
  s_box3_layer = layer_create(start_number_box); 
  //layer_add_child(s_canvas_layer, s_box3_layer);
  layer_add_child(window_layer, s_box3_layer);
  
  s_box4_layer = layer_create(start_number_box);
  //layer_add_child(s_canvas_layer, s_box4_layer);
  layer_add_child(window_layer, s_box4_layer);
  
  
  window_set_background_color(s_main_window, GColorBlack);
  
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  layer_set_update_proc(s_box1_layer, box1_update_proc);
  layer_set_update_proc(s_box2_layer, box2_update_proc);
  layer_set_update_proc(s_box3_layer, box3_update_proc);
  layer_set_update_proc(s_box4_layer, box4_update_proc);
  
  //s_time_middle_layer = layer_create(GRect(-5,60,5,60));
  //layer_set_update_proc(s_time_middle_layer, middle_layer_update_proc);
  //layer_add_child(window_layer, s_time_middle_layer);
  
}

static void main_window_unload(Window *window){
  
  layer_destroy(s_box1_layer);
  layer_destroy(s_box2_layer);
  layer_destroy(s_box3_layer);
  layer_destroy(s_box4_layer);
  
  //layer_destroy(s_time_middle_layer);
  bitmap_layer_destroy(s_camo_bg_layer);
  layer_destroy(s_canvas_layer);
  gbitmap_destroy(s_camo_bitmap);
  
}
   
static void init(void){     // set up layers/windows
    
  number_color = GColorWhite;
  shadow_color = GColorBlack;
  
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  
  //s_boxpath = gpath_create(&BOX_PATH);  // initialize the path info for the time dots
  
  update_time();
  
  //set up initial gpaths
  s_number1_path = gpath_create(time_digit_info((int)floor(currentHour / 10)));
  s_number2_path = gpath_create(time_digit_info(currentHour % 10));
  s_number3_path = gpath_create(time_digit_info((int)floor(currentMinute / 10)));
  s_number4_path = gpath_create(time_digit_info(currentMinute % 10));
   
  digits_changed_during_tick[0] = true;
  digits_changed_during_tick[1] = true;
  digits_changed_during_tick[2] = true;
  digits_changed_during_tick[3] = true;
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  GPoint number_starting_points[4] = {{0,0}};
  for (int i=1; i<4; i++){
    number_starting_points[i] = number_point_setup(number_starting_points[0], NUMBER_SPACING, CENTER_SPACING_CONSTANT, i, M_PI / 7);
    //APP_LOG(APP_LOG_LEVEL_INFO,"Point %d: (%d, %d)",i, number_starting_points[i].x,number_starting_points[i].y);
  }
  
  app_message_register_inbox_received(inbox_received_handler);
  //app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_minimum());
  app_message_open(256, 256);
  
}

static void deinit(void){   //destroy layers/windows, etc.

  tick_timer_service_unsubscribe();
  //gpath_destroy(s_boxpath);
  
  gpath_destroy(s_monthday1_path);
  gpath_destroy(s_monthday2_path);
    
  clean_up_number_gpaths();           // destroy all gpaths for numbers
  animation_unschedule_all();  // Stop any animation in progress
  window_destroy(s_main_window);  // Destroy main Window
  
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
