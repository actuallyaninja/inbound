#include <pebble.h>

// rotate a gpoint around the origin
GPoint rotate_gpoint_about_origin(GPoint point, int32_t angle){
  
  return GPoint(
    ((point.x) * cos_lookup(angle) - (point.y) * sin_lookup(angle)),
    ((point.x) * sin_lookup(angle) + (point.y) * cos_lookup(angle))
  );
  
}

// rotate a gpoint around another point
// orig is the point that serves as the origin of rotation
// point is the point that will be rotated
// angle is the angle of rotation
GPoint rotate_gpoint(GPoint orig, GPoint point, int32_t angle){
  
  return GPoint(
      cos_lookup(angle)*(point.x - orig.x) - sin_lookup(angle)*(point.y - orig.y) + orig.x,                      
      sin_lookup(angle)*(point.x - orig.x) + cos_lookup(angle)*(point.y - orig.y) + orig.y
  );
  
}

GPoint rotate_pbl_gpoint(GPoint orig, GPoint point, int32_t angle){
  
  return GPoint(
      cos_lookup(angle)*(point.x - orig.x) - sin_lookup(angle)*(point.y - orig.y) + orig.x,                      
      sin_lookup(angle)*(point.x - orig.x) + cos_lookup(angle)*(point.y - orig.y) + orig.y
  );
  
}

GPoint rotate_gpoint_around_watchface_center(GPoint point, int32_t angle){
  #ifdef PBL_PLATFORM_BASALT
    return rotate_gpoint(GPoint(72,84),point,angle);
  #elif PBL_PLATFORM_APLITE
    return rotate_gpoint(GPoint(72,84),point,angle);
  #elif PBL_ROUND
    return rotate_gpoint(GPoint(90,90), point, angle);
  #endif
}

