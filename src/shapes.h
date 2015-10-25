#pragma once
  
#include <pebble.h>

#define STANDARD_DIGIT_WIDTH 25
#define STANDARD_DIGIT_HEIGHT 60

//define shapes for digits and dots
 
//set up a box to be used for time dots
static const GPathInfo BOX_PATH = {
  4,
  (GPoint[]) {{0,0},{24,0},{24,59},{0,59}}
};

//number 0:
static const GPathInfo ZERO_PATH = {
  10,
  (GPoint []) {{0,0},{24,0},{24,59},{0,59},{0,0},{9,0},{9,50},{15,50},{15,9},{0,9}}
};

// number 1:
static const GPathInfo ONE_PATH = {
  6,
  (GPoint []) {{8,0},{24,0},{24,59},{14,59},{14,9},{8,9}}
};

//number 2:
static const GPathInfo TWO_PATH = {
  12,
  (GPoint []) {{0,0},{24,0},{24,35},{9,35},{9,47},{24,47},{24,59},{0,59},{0,24},{14,24},{14,9},{0,9}}
};

//number 3:
static const GPathInfo THREE_PATH = {
  12,
  (GPoint []) {{0,0},{24,0},{24,59},{0,59},{0,47},{14,47},{14,35},{0,35},{0,24},{14,24},{14,11},{0,11}}
};

//number 4:
static const GPathInfo FOUR_PATH = {
  10,
  (GPoint []) {{0,0},{9,0},{9,24},{15,24},{15,0},{24,0},{24,59},{14,59},{14,35},{0,35}}
};

//number 5:
static const GPathInfo FIVE_PATH = {
  12,
  (GPoint []) {{0,0},{24,0},{24,11},{9,11},{9,24},{24,24},{24,59},{0,59},{0,48},{14,48},{14,35},{0,35}}
};

//number 6:
static const GPathInfo SIX_PATH = {
  12,
  (GPoint []) {{0,0},{24,0},{24,11},{9,11},{9,48},{15,48},{15,35},{0,35},{0,24},{24,24},{24,59},{0,59}}
};

//number 7:
static const GPathInfo SEVEN_PATH = {
  6,
  (GPoint []) {{0,0},{24,0},{14,59},{5,59},{13,11},{0,11}}
};

//number 8:
static const GPathInfo EIGHT_PATH = {
  14,
  (GPoint []) {{0,25},{0,0},{24,0},{24,59},{0,59},{0,24},{23,24},{23,35},{9,35},{9,48},{15,48},{15,11},{9,11},{9,25}}
};

//number 9:
static const GPathInfo NINE_PATH = {
  12,
//  (GPoint []) {{15,24},{15,35},{0,35},{0,0},{24,0},{24,59},{0,59},{0,47},{15,47},{15,11},{9,11},{9,24}}
  (GPoint []) {{0,0},{24,0},{24,59},{0,59},{0,47},{15,47},{15,11},{9,11},{9,24},  {24,24},{24,35},  {0,35}}
};

// first month_day_digit
static GPathInfo DAY_DIGIT_1 = {14, (GPoint[]) {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}} };
static GPathInfo DAY_DIGIT_2 = {14, (GPoint[]) {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}} };

static const GPathInfo *ptr_day_digit_1 = &DAY_DIGIT_1;
static const GPathInfo *ptr_day_digit_2 = &DAY_DIGIT_2;

//create number info objects
static const GPathInfo *itszero = &ZERO_PATH;
static const GPathInfo *itsone = &ONE_PATH;
static const GPathInfo *itstwo = &TWO_PATH;
static const GPathInfo *itsthree = &THREE_PATH;
static const GPathInfo *itsfour = &FOUR_PATH;
static const GPathInfo *itsfive = &FIVE_PATH;
static const GPathInfo *itssix = &SIX_PATH;
static const GPathInfo *itsseven = &SEVEN_PATH;
static const GPathInfo *itseight = &EIGHT_PATH;
static const GPathInfo *itsnine = &NINE_PATH;

// translate a number into the gpath_info for that number
const GPathInfo *time_digit_info(int8_t digit){
  switch (digit){
    case 1:
      return itsone;
    case 2:
      return itstwo;
    case 3:
      return itsthree;
    case 4:
      return itsfour;
    case 5:
      return itsfive;
    case 6:
      return itssix;
    case 7:
      return itsseven;
    case 8: 
      return itseight;
    case 9:
      return itsnine;
    case 0:
      return itszero;
    default:
      return itszero;
  }
}

/*
GPathInfo new_gpathinfo_from_existing(GPathInfo existing){
  GPoint existing_points[14] = {GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0),GPoint(0,0)};
    
  //temp.num_points = existing.num_points;
  
  for (uint32_t i = 0; i < existing.num_points; i++){
    existing_points[i].x = existing.points[i].x;
    existing_points[i].y = existing.points[i].y;
    APP_LOG(APP_LOG_LEVEL_INFO,"temp.points[%d] being set to (%d,%d)...",(int)i,(int)existing.points[i].x,(int)existing.points[i].y);
  }
   
  GPathInfo temp = {14, existing_points}; // = malloc(sizeof(GPathInfo));
  
  return temp;
}
*/

static GPathInfo time_digit_info_value(int8_t digit){
  //GPathInfo temp;
    
  switch (digit){
    case 1:
      return ONE_PATH;
      //temp = ONE_PATH;
      break;
    case 2:
      return TWO_PATH;
      //temp = TWO_PATH;
      //return (GPathInfo){TWO_PATH.num_points, TWO_PATH.points};
      //return new_gpathinfo_from_existing(TWO_PATH);
      break;
    case 3:
      return THREE_PATH;
      //temp = THREE_PATH;
      break;
    case 4:
      return FOUR_PATH;
      //temp = FOUR_PATH;
      break;
    case 5:
      return FIVE_PATH;
      //temp = FIVE_PATH;
      break;
    case 6:
      return SIX_PATH;
      //temp = SIX_PATH;
      break;
    case 7:
      return SEVEN_PATH;
      //temp = SEVEN_PATH;
      break;
    case 8: 
      return EIGHT_PATH;
      //temp = EIGHT_PATH;
      break;
    case 9:
      return NINE_PATH;
      //temp = NINE_PATH;
      break;
    case 0:
      return ZERO_PATH;
      //temp = ZERO_PATH;
      break;
    default:
      return ZERO_PATH;
      //temp = ZERO_PATH;
      break;
  }
  
  //GPathInfo result = (GPathInfo){temp.num_points, temp.points};
  
  //return new_gpathinfo_from_existing(TWO_PATH);
}


