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

