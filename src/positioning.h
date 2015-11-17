#pragma once

#include <pebble.h>
#include <extramath.h>

#define NUMBER_SPACING 30
#define CENTER_SPACING_CONSTANT 12

//float rotation_angle = M_PI / 7; // M_PI / 7 seems to be roughly equivalent to the way these are positioned manually
//GPoint number_point_layout[4] = {{0,0}};

GPoint rotate_gpoint(GPoint orig, GPoint point, float angle){
  
  return GPoint(
      my_cos(angle)*(point.x - orig.x) - my_sin(angle)*(point.y - orig.y) + orig.x,
      my_sin(angle)*(point.x - orig.x) + my_cos(angle)*(point.y - orig.y) + orig.y
  );
  
}

GPoint number_point_setup(GPoint origin, int default_spacing, int center_spacing, int position_number, float rotation_angle){
  // start with a point (x-int and y-int) and project 4 points evenly spaced in a line
  // start with 4 points evenly spaced on a horizontal line....
  //width spacing is based on a constant that is the width of the number plus some padding for space between numbers
  GPoint my_return = origin;
  
  if (position_number < 2){
    my_return = GPoint((position_number * default_spacing) + origin.x, origin.y);
  } else{
     my_return = GPoint((position_number * default_spacing)  + origin.x + center_spacing, origin.y);
  }
  
  // rotate them
  my_return = rotate_gpoint(origin, my_return, rotation_angle);
  
  
  
  return my_return;
}


/* 
current positions resulting from existing code:

number positions:

tilt = 1 (slanted right)

w/ 4 digits (normal)
digit 1: (0,25)
digit 2: (26,38)
digit 3: (65,55)
digit 4: (91,68)

w/ 4 digits (starting w/ "1")
digit 1: (-5,25)
digit 2: (21,38)
digit 3: (60,55)
digit 4: (86,68)

w/3 digits (normal)
digit 1: (-13,19) //not used
digit 2: (13,32)
digit 3: (52,49)
digit 4: (78,62)

this configuration doesn't correct for the 1 at the beginning...still looks fine...
w/3 digits (starting w/ "1")
digit 1: (-13,19) //not used
digit 2: (13,32)
digit 3: (52,49)
digit 4: (78,62)



tilt = -1 (slanted left)

w/ 4 digits (normal)
digit 1: (3,72)
digit 2: (29,59)
digit 3: (68,42)
digit 4: (94,29)

w/ 4 digits (starting w/ "1")
digit 1: (-2,72)
digit 2: (24,59)
digit 3: (63,42)
digit 4: (89,29)

w/ 3 digits (normal)
digit 1: (-13,78) // not used
digit 2: (13,65)
digit 3: (52,48)
digit 4: (78,35)

this configuration doesn't correct for the 1 at the beginning...still looks fine...
w/3 digits (starting w/ "1")
digit 1: (-13,78) //not used
digit 2: (13,65)
digit 3: (52,48)
digit 4: (78,35)


round offsets:
#ifdef PBL_ROUND
  x_offset += 18;
  y_offset += 6;
#endif

*/