#pragma once
  
#include <pebble.h>

#define NUM_PALETTE_COLORS 7
#define NUM_PALETTES 8
//#define CHEVRON_HEIGHT 30
#define CHEVRON_HEIGHT 32
#ifdef PBL_ROUND
#define SCREEN_WIDTH 180
#define SCREEN_HEIGHT 180
#define CHEVRON_MID_Y 40
#define CHEVRON2_MID_Y 
#else
#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define CHEVRON_MID_Y 47
#endif


static const GPathInfo CHEVRON = {
  6,
  #ifdef PBL_ROUND
  //(GPoint[]) {{0,0},{0,30},{90,72},{181,30},{181,0},{90,42}}
  (GPoint[]) {{0,0},{0,32},{90,74},{181,32},{181,0},{90,42}}
  #else
  //(GPoint[]) {{0,0},{0,30},{72,64},{145,30},{145,0},{72,34}}
  (GPoint[]) {{0,0},{0,32},{72,66},{145,32},{145,0},{72,34}}
  #endif
};

#ifdef PBL_COLOR

static uint8_t PALETTES[NUM_PALETTES][NUM_PALETTE_COLORS] = {
  {GColorWhiteARGB8,GColorRedARGB8,GColorBlueARGB8,GColorBlueARGB8,
                                      GColorBlueARGB8,GColorRedARGB8,GColorWhiteARGB8},
  {GColorRichBrilliantLavenderARGB8,GColorBrilliantRoseARGB8,GColorRedARGB8,GColorPurpleARGB8,
                                      GColorCobaltBlueARGB8,GColorVividCeruleanARGB8,GColorCelesteARGB8},
  {GColorIcterineARGB8,GColorYellowARGB8,GColorChromeYellowARGB8,GColorOrangeARGB8,
                                      GColorRedARGB8,GColorDarkCandyAppleRedARGB8,GColorBulgarianRoseARGB8},
  {GColorBrassARGB8,GColorLimerickARGB8,GColorWindsorTanARGB8,GColorArmyGreenARGB8,
                                      GColorDarkGreenARGB8,GColorJaegerGreenARGB8,GColorMayGreenARGB8},
  {GColorBulgarianRoseARGB8,GColorDarkCandyAppleRedARGB8,GColorOrangeARGB8,GColorChromeYellowARGB8,
                                      GColorMidnightGreenARGB8,GColorDukeBlueARGB8,GColorIndigoARGB8},    
  {GColorDarkGreenARGB8,GColorJaegerGreenARGB8,GColorOrangeARGB8,GColorChromeYellowARGB8,
                                      GColorVividVioletARGB8,GColorDarkCandyAppleRedARGB8,GColorImperialPurpleARGB8},
  {GColorWhiteARGB8,GColorLightGrayARGB8,GColorDarkGrayARGB8,GColorBlackARGB8,
                                      GColorDarkGrayARGB8,GColorLightGrayARGB8,GColorWhiteARGB8},
  {GColorRedARGB8,GColorDarkGrayARGB8,GColorBlackARGB8,GColorRedARGB8,
                                      GColorDarkGrayARGB8,GColorBlackARGB8,GColorRedARGB8}
};

#else
int PALETTES[NUM_PALETTES][NUM_PALETTE_COLORS] = {
{0,-1,0,0,0,-1,0},
  {1,-1,0,0,0,-1,1},
  {0,-1,-1,0,-1,-1,0},
  {1,0,-1,-1,-1,0,1},
  //{0,-1,-1,-1,-1,-1,0},
  {0,0,-1,-1,-1,-1,0},
  {1,-1,0,-1,0,-1,1},
  {-1,-1,-1,0,-1,-1,-1},
  {-1,0,-1,0,-1,0,-1}
};

#endif