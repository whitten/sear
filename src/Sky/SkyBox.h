// This file may be redistributed and modified only under the terms of
// // the GNU General Public License (See COPYING for details).
// // Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton
 
#ifndef _SKYBOX_H_
#define _SKYBOX_H_ 1

#include "System.h"
#include "Render.h"

#define NUM_SKY_TEXTURES 12

// defines for Config key values

#define CONFIG_SKYDIST "SkyDist"
#define CONFIG_SKY_DAY_NORTH   "SkyDayTextureNorth"
#define CONFIG_SKY_DAY_EAST    "SkyDayTextureEast"
#define CONFIG_SKY_DAY_SOUTH   "SkyDayTextureSouth"
#define CONFIG_SKY_DAY_WEST    "SkyDayTextureWest"
#define CONFIG_SKY_DAY_TOP     "SkyDayTextureTop"
#define CONFIG_SKY_DAY_BOTTOM  "SkyDayTextureBottom"

#define CONFIG_SKY_NIGHT_NORTH   "SkyNightTextureNorth"
#define CONFIG_SKY_NIGHT_EAST    "SkyNightTextureEast"
#define CONFIG_SKY_NIGHT_SOUTH   "SkyNightTextureSouth"
#define CONFIG_SKY_NIGHT_WEST    "SkyNightTextureWest"
#define CONFIG_SKY_NIGHT_TOP     "SkyNightTextureTop"
#define CONFIG_SKY_NIGHT_BOTTOM  "SkyNightTextureBottom"

// Define texture locations 

#define TEXTURE_SKY_DAY_NORTH  0
#define TEXTURE_SKY_DAY_SOUTH  1
#define TEXTURE_SKY_DAY_EAST   2
#define TEXTURE_SKY_DAY_WEST   3
#define TEXTURE_SKY_DAY_TOP    4
#define TEXTURE_SKY_DAY_BOTTOM 5

#define TEXTURE_SKY_NIGHT_NORTH  6
#define TEXTURE_SKY_NIGHT_SOUTH  7
#define TEXTURE_SKY_NIGHT_EAST   8
#define TEXTURE_SKY_NIGHT_WEST   9
#define TEXTURE_SKY_NIGHT_TOP    10
#define TEXTURE_SKY_NIGHT_BOTTOM 11


#define VERTEX_COORDS { \
  -1.0f,  1.0f,  1.0f,  \
  -1.0f, -1.0f,  1.0f,  \
   1.0f, -1.0f,  1.0f,  \
   1.0f,  1.0f,  1.0f,  \
                        \
  -1.0f,  1.0f, -1.0f,  \
  -1.0f,  1.0f,  1.0f,  \
   1.0f,  1.0f,  1.0f,  \
   1.0f,  1.0f, -1.0f,  \
                        \
   1.0f, -1.0f, -1.0f,  \
   1.0f, -1.0f,  1.0f,  \
  -1.0f, -1.0f,  1.0f,  \
  -1.0f, -1.0f, -1.0f,  \
                        \
   1.0f,  1.0f, -1.0f,  \
   1.0f,  1.0f,  1.0f,  \
   1.0f, -1.0f,  1.0f,  \
   1.0f, -1.0f, -1.0f,  \
                        \
  -1.0f ,-1.0f, -1.0f,  \
  -1.0f, -1.0f,  1.0f,  \
  -1.0f,  1.0f,  1.0f,  \
  -1.0f,  1.0f, -1.0f,  \
                        \
  -1.0f, -1.0f, -1.0f,  \
  -1.0f,  1.0f, -1.0f,  \
   1.0f,  1.0f, -1.0f,  \
   1.0f, -1.0f, -1.0f }

#define TEXTURE_COORDS {  \
  0, 0, 0, 1, 1, 1, 1, 0, \
  0, 0, 0, 1, 1, 1, 1, 0, \
  0, 0, 0, 1, 1, 1, 1, 0, \
  0, 0, 0, 1, 1, 1, 1, 0, \
  0, 0, 0, 1, 1, 1, 1, 0, \
  0, 0, 0, 1, 1, 1, 1, 0 }

#define NORMAL_COORDS { \
   0.0f,  0.0f, -1.0f, \
   0.0f, -1.0f,  0.0f, \
   0.0f,  1.0f,  0.0f, \
  -1.0f,  0.0f,  0.0f, \
   1.0f,  0.0f,  0.0f, \
   0.0f,  0.0f,  1.0f }

// Forward Decls
class Render;
  
class SkyBox {
public:
  SkyBox(System *, Render*);
  ~SkyBox();
  
  bool init();
  void shutdown();
  
  void draw();

protected:
  /**
    This method will draw the SkyBox
  */
  void render();
  void renderTextureSet(int);

  static float vertex_coords[];
  static int texture_coords[];
  static float normal_coords[];
 
  int texture_id[NUM_SKY_TEXTURES];
 
  System *_system; 
  Render *_renderer;
};

#endif /* _SKYBOX_H_ */
