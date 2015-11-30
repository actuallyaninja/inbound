#pragma once
  
#include <pebble.h>

#define NUM_PALETTE_COLORS 7
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


static const GPathInfo CHEVRON2 = {
  6,
  (GPoint[]) {{0,0},{0,30},{200,126},{400,30},{400,0},{200,96}}
};

#ifdef PBL_COLOR
/*
static const uint8_t PALETTE_REDBLUE[7] = {GColorRichBrilliantLavender,GColorBrilliantRose,
                                    GColorRed,GColorPurple,GColorBlue,GColorVividCerulean,GColorCeleste};
static const uint8_t PALETTE_YELLOWPURP[7] = {GColorIcterine,GColorYellow,GColorChromeYellow,GColorOrange,
                                       GColorRed,GColorDarkCandyAppleRed,GColorImperialPurple};
static const uint8_t PALETTE_GREENTAN[7] = {GColorBrass,GColorLimerick,GColorWindsorTan,GColorArmyGreen,
                                     GColorDarkGreen,GColorJaegerGreen,GColorMayGreen};
 */ 

static const uint8_t PALETTES[3][7] = {
  {GColorRichBrilliantLavenderARGB8,GColorBrilliantRoseARGB8,GColorRedARGB8,GColorPurpleARGB8,
                                      GColorBlueARGB8,GColorVividCeruleanARGB8,GColorCelesteARGB8},
  {GColorIcterineARGB8,GColorYellowARGB8,GColorChromeYellowARGB8,GColorOrangeARGB8,
                                      GColorRedARGB8,GColorDarkCandyAppleRedARGB8,GColorImperialPurpleARGB8},
  {GColorBrassARGB8,GColorLimerickARGB8,GColorWindsorTanARGB8,GColorArmyGreenARGB8,
                                      GColorDarkGreenARGB8,GColorJaegerGreenARGB8,GColorMayGreenARGB8}
  
};

//static  GColor8 PALETTE_REDBLUE[7] = {{GColorRichBrilliantLavenderARGB8},{GColorBrilliantRoseARGB8},{GColorRedARGB8},{GColorPurpleARGB8},{GColorBlueARGB8},{GColorVividCeruleanARGB8},{GColorCelesteARGB8}};
//static  GColor8 PALETTE_YELLOWPURP[7] = {{GColorIcterineARGB8},{GColorYellowARGB8},{GColorChromeYellowARGB8},{GColorOrangeARGB8},{GColorRedARGB8},{GColorDarkCandyAppleRedARGB8},{GColorImperialPurpleARGB8}};
//static  GColor8 PALETTE_GREENTAN[7] = {{GColorBrassARGB8},{GColorLimerickARGB8},{GColorWindsorTanARGB8},{GColorArmyGreenARGB8},{GColorDarkGreenARGB8},{GColorJaegerGreenARGB8},{GColorMayGreenARGB8}};

#define REDBLUE_PALETTE {GColorRichBrilliantLavender,GColorBrilliantRose,GColorRed,GColorPurple,GColorBlue,GColorVividCerulean,GColorCeleste}
#define YELLOWPURP_PALETTE {GColorIcterine,GColorYellow,GColorChromeYellow,GColorOrange,GColorRed,GColorDarkCandyAppleRed,GColorImperialPurple}
#define GREENTAN_PALETTE {GColorBrass,GColorLimerick,GColorWindsorTan,GColorArmyGreen,GColorDarkGreen,GColorJaegerGreen,GColorMayGreen}

#define REDBLUE_PALETTE_ARGB8 {{GColorRichBrilliantLavenderARGB8},{GColorBrilliantRoseARGB8},{GColorRedARGB8},{GColorPurpleARGB8},{GColorBlueARGB8},{GColorVividCeruleanARGB8},{GColorCelesteARGB8}}
#define YELLOWPURP_PALETTE_ARGB8 {{GColorIcterineARGB8},{GColorYellowARGB8},{GColorChromeYellowARGB8},{GColorOrangeARGB8},{GColorRedARGB8},{GColorDarkCandyAppleRedARGB8},{GColorImperialPurpleARGB8}}
#define GREENTAN_PALETTE_ARGB8 {{GColorBrassARGB8},{GColorLimerickARGB8},{GColorWindsorTanARGB8},{GColorArmyGreenARGB8},{GColorDarkGreenARGB8},{GColorJaegerGreenARGB8},{GColorMayGreenARGB8}}

#endif