#pragma once
  
#include <pebble.h>
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
