// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _GL_RENDER_H_
#define _GL_RENDER_H_ 1

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <string>
#include <list>
#include <map>

#include "../src/StateLoader.h"

#include "../src/Render.h"

#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>

namespace Sear {

class System;
class Terrain;
class Sky;
class Camera;
class WorldEntity;
class Client;
class Character;
class Models;
class ModelHandler;

class GL : public Render {

typedef std::list<WorldEntity*> Queue;
	
public:
  GL();
  GL(System *);
  
  ~GL();
  void init();
  void initWindow(int width, int height);
  void shutdown() {};

  void initLighting();
  void initFont();
  void shutdownFont();
  void print(GLint x, GLint y, const char*, int set);
  void print3D(const char* string, int set);
  void newLine();

  int requestTexture(const std::string&, bool clamp = false);
  int requestMipMap(const std::string &, bool clamp = false);
  int requestTextureMask(const std::string&, bool clamp = false);
  void createTexture(SDL_Surface*, unsigned int, bool);
  void createMipMap(SDL_Surface*, unsigned int, bool);
  void createTextureMask(SDL_Surface*, unsigned int, bool);
  
  void initModels();
  GLuint getTextureID(int texture_id);
  static GL *instance() { return _instance; }
  void buildColourSet();
  void drawScene(const std::string &,bool);
  void drawTextRect(int, int, int, int, int);
  void stateChange(const std::string &state);
  void stateChange(StateProperties *state);
  float distFromNear(float,float,float);  
  void setColour(float red, float green, float blue, float alpha) { glColor4f(red, green, blue, alpha); }

  int patchInFrustum(WFMath::AxisBox<3>);
  
  void procEvent(int, int);
  int getWindowWidth() { return window_width; }
  int getWindowHeight() { return window_height; }

  void switchTexture(int texture);// { glBindTexture(GL_TEXTURE_2D, getTextureID(texture));}
  void switchTextureID(unsigned int texture) ;//{ glBindTexture(GL_TEXTURE_2D, texture);}
void createDefaults();
  
  std::string getActiveID() { return activeID; }
  Camera* getCamera() { return camera; }
  Terrain* getTerrain() { return terrain; }
  Sky* getSkyBox() { return skybox; }
  void checkModelStatus(const std::string &) {}
  void setModelInUse(const std::string &, bool) {}

  void readConfig();
  void writeConfig();
  void setupStates();
  void readComponentConfig();
  void writeComponentConfig();

  float getLightLevel() { return _light_level; }


  void translateObject(float x, float y, float z);
  void rotateObject(WorldEntity *we, int type);
  void setViewMode(int type);
  void setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive);
  void renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, float *vertex_data, float *texture_data, float *normal_data);
  void renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, float *vertex_data, float *texture_data, float *normal_data);
  unsigned int createTexture(unsigned int width, unsigned int height, unsigned int depth, unsigned char *data, bool clamp);
  void drawQueue(std::map<std::string, Queue> queue, bool select_mode, float time_elapsed);
  void drawMessageQueue(std::map<std::string, Queue> queue, bool select_mode);

//  static WFMath::AxisBox<3> bboxCheck(WFMath::AxisBox<3> bbox);
  void drawOutline(WorldEntity *, Models *, bool);
  
protected:
  System *_system;
  int window_width;
  int window_height;

  Uint32 time;
  
  const float fov;
  const float near_clip;
  const float far_clip;
  float _far_clip_dist;

  float frustum[6][4];

  std::map<std::string, int> texture_map;
  
  int next_id;
  GLuint base;
  std::list<GLuint> textureList;

  int font_id;
  int splash_id;
  static GL *_instance;

  std::string activeID;
  std::string active_name;
  int x_pos;
  int y_pos;
  
  Terrain *terrain;
  Sky *skybox;
  Camera *camera;
  
  Client *client;
  Models *player_model;
  
  int num_frames;
  float frame_time;
  std::map<std::string, Queue> render_queue;

  void buildQueues(WorldEntity*, int, bool);
  StateLoader *_state_loader; 
  float frame_rate;
  float model_detail;

  WFMath::Quaternion orient;
 
  typedef struct {
//    light id;
    float kc;
    float kl;
    float kq;
    float x_pos;
    float y_pos;
    float z_pos;
    float w_pos;
    float ambient[4];
    float diffuse[4];
    float specular[4];
//    colour;
//    spotlight stuff;
  } lightStruct;

  typedef enum {
    LIGHT_CHARACTER = 0,
    LIGHT_SUN,
    LIGHT_LAST
  } lightSources;

  lightStruct lights[LIGHT_LAST];
  GLuint states;
  GLuint _states;

  float _speech_offset_x;
  float _speech_offset_y;
  float _speech_offset_z;
 
  float _lower_frame_rate_bound;
  float _upper_frame_rate_bound;

  Character *_character;

  
  void CheckError();

  std::string _current_state;
  StateProperties *_cur_state;

  void stateDisplayList(GLuint &, StateProperties *previous_state, StateProperties *next_state);

  float _fog_start;
  float _fog_end;
  float _light_level;
  bool _depth_buffer_flag;

  static float _halo_blend_colour[4];// = {0.0f, 0.0f, 1.0f, 0.4f};
  static float _halo_colour[3];// = {0.0f, 0.0f, 1.0f};


  ModelHandler *mh;
  std::map<std::string, GLuint> _state_map;
private:
  // Consts
  static const int sleep_time = 5000;

  static const char * const font_texture = "ui_font";
  static const char * const splash_texture = "ui_splash";
  
  // Config key strings
  
  static const char * const KEY_use_textures = "render_use_textures";
  static const char * const KEY_use_lighting = "render_use_lighting";
  static const char * const KEY_show_fps = "render_show_fps";
  static const char * const KEY_use_stencil = "render_use_stencil";

  static const char * const KEY_character_light_kc = "character_light_kc";
  static const char * const KEY_character_light_kl = "character_light_kl";
  static const char * const KEY_character_light_kq = "character_light_kq";
  
  static const char * const KEY_character_light_ambient_red = "character_light_ambient_red";
  static const char * const KEY_character_light_ambient_blue = "character_light_ambient_blue";
  static const char * const KEY_character_light_ambient_green = "character_light_ambient_green";
  static const char * const KEY_character_light_ambient_alpha = "character_light_ambient_alpha";

  static const char * const KEY_character_light_diffuse_red = "character_light_diffuse_red";
  static const char * const KEY_character_light_diffuse_blue = "character_light_diffuse_blue";
  static const char * const KEY_character_light_diffuse_green = "character_light_diffuse_green";
  static const char * const KEY_character_light_diffuse_alpha = "character_light_diffuse_alpha";
  
  static const char * const KEY_character_light_specular_red = "character_light_specular_red";
  static const char * const KEY_character_light_specular_blue = "character_light_specular_blue";
  static const char * const KEY_character_light_specular_green = "character_light_specular_green";
  static const char * const KEY_character_light_specular_alpha = "character_light_specular_alpha";
  
  static const char * const KEY_sun_light_kc = "sun_light_kc";
  static const char * const KEY_sun_light_kl = "sun_light_kl";
  static const char * const KEY_sun_light_kq = "sun_light_kq";
  
  static const char * const KEY_sun_light_specular_red = "sun_light_specular_red";
  static const char * const KEY_sun_light_specular_blue = "sun_light_specular_blue";
  static const char * const KEY_sun_light_specular_green = "sun_light_specular_green";
  static const char * const KEY_sun_light_specular_alpha = "sun_light_specular_alpha";

  static const char * const KEY_lower_frame_rate_bound = "lower_frame_rate_bound";
  static const char * const KEY_upper_frame_rate_bound = "upper_frame_rate_bound";
  
  static const char * const KEY_speech_offset_x = "speech_offset_x";
  static const char * const KEY_speech_offset_y = "speech_offset_y";
  static const char * const KEY_speech_offset_z = "speech_offset_z";
  
  static const char * const KEY_fog_start = "fog_start";
  static const char * const KEY_fog_end = "fog_end";

  static const char * const KEY_far_clip_dist = "far_clip_dist";
  
  // Default config values
  static const float DEFAULT_character_light_kc = 1.0f;
  static const float DEFAULT_character_light_kl = 0.0f;
  static const float DEFAULT_character_light_kq = 0.0f;

  static const float DEFAULT_character_light_ambient_red = 0.0f;
  static const float DEFAULT_character_light_ambient_green = 0.0f;
  static const float DEFAULT_character_light_ambient_blue = 0.0f;
  static const float DEFAULT_character_light_ambient_alpha = 0.0f;
  
  static const float DEFAULT_character_light_diffuse_red = 0.0f;
  static const float DEFAULT_character_light_diffuse_green = 0.0f;
  static const float DEFAULT_character_light_diffuse_blue = 0.0f;
  static const float DEFAULT_character_light_diffuse_alpha = 0.0f;

  static const float DEFAULT_character_light_specular_red = 0.0f;
  static const float DEFAULT_character_light_specular_green = 0.0f;
  static const float DEFAULT_character_light_specular_blue = 0.0f;
  static const float DEFAULT_character_light_specular_alpha = 0.0f;
  
  static const float DEFAULT_sun_light_kc = 1.0f;
  static const float DEFAULT_sun_light_kl = 0.0f;
  static const float DEFAULT_sun_light_kq = 0.0f;

  static const float DEFAULT_sun_light_specular_red = 0.0f;
  static const float DEFAULT_sun_light_specular_green = 0.0f;
  static const float DEFAULT_sun_light_specular_blue = 0.0f;
  static const float DEFAULT_sun_light_specular_alpha = 0.0f;

  static const float DEFAULT_use_textures = true;
  static const float DEFAULT_use_lighting = true;
  static const float DEFAULT_show_fps = true;
  static const float DEFAULT_use_stencil = true;

  static const float DEFAULT_lower_frame_rate_bound = 25.0f;
  static const float DEFAULT_upper_frame_rate_bound = 30.0f;

  static const float DEFAULT_speech_offset_x = 0.0f;
  static const float DEFAULT_speech_offset_y = 0.0f;
  static const float DEFAULT_speech_offset_z = 0.0f;

  static const float DEFAULT_fog_start = 20.0f;
  static const float DEFAULT_fog_end = 35.0f;
  static const float DEFAULT_far_clip_dist = 100.0f;
};

} /* namespace Sear */
#endif /* _GL_RENDER_H_ */
