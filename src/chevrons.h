#pragma once
  
#include <pebble.h>

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
  5,
  (GPoint[]) {{0,0},{0,CHEVRON_MID_Y},{SCREEN_WIDTH/2,83},{SCREEN_WIDTH,CHEVRON_MID_Y},{SCREEN_WIDTH,0}}
};

static const GPathInfo CHEVRON2 = {
  6,
  (GPoint[]) {{0,0},{0,30},{200,126},{400,30},{400,0},{200,96}}
};

#ifdef PBL_COLOR
static GColor8 PALETTE_REDBLUE[7] = {{GColorRichBrilliantLavenderARGB8},{GColorBrilliantRoseARGB8},{GColorRedARGB8},{GColorPurpleARGB8},{GColorBlueARGB8},{GColorVividCeruleanARGB8},{GColorCelesteARGB8}};
static GColor8 PALETTE_YELLOWPURP[7] = {{GColorIcterineARGB8},{GColorYellowARGB8},{GColorChromeYellowARGB8},{GColorOrangeARGB8},{GColorRedARGB8},{GColorDarkCandyAppleRedARGB8},{GColorImperialPurpleARGB8}};
static GColor8 PALETTE_GREENTAN[7] = {{GColorBrassARGB8},{GColorLimerickARGB8},{GColorWindsorTanARGB8},{GColorArmyGreenARGB8},{GColorDarkGreenARGB8},{GColorJaegerGreenARGB8},{GColorMayGreenARGB8}};
//static GColor8 PALETTES[3]

#define REDBLUE_PALETTE {{GColorRichBrilliantLavenderARGB8},{GColorBrilliantRoseARGB8},{GColorRedARGB8},{GColorPurpleARGB8},{GColorBlueARGB8},{GColorVividCeruleanARGB8},{GColorCelesteARGB8}}
#define YELLOWPURP_PALETTE {{GColorIcterineARGB8},{GColorYellowARGB8},{GColorChromeYellowARGB8},{GColorOrangeARGB8},{GColorRedARGB8},{GColorDarkCandyAppleRedARGB8},{GColorImperialPurpleARGB8}}
#define GREENTAN_PALETTE {{GColorBrassARGB8},{GColorLimerickARGB8},{GColorWindsorTanARGB8},{GColorArmyGreenARGB8},{GColorDarkGreenARGB8},{GColorJaegerGreenARGB8},{GColorMayGreenARGB8}}

#endif