// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: ObjectLoader.h,v 1.16 2002-09-08 00:24:53 simon Exp $

#ifndef SEAR_OBJECTLOADER_H
#define SEAR_OBJECTLOADER_H 1

#include "System.h"
#include <string>
#include <map>
#include <stdio.h>


namespace Sear {
  static const unsigned int OBJECT_LOADER_MAX_SIZE = 128;
	
typedef struct {
  float ambient[4];
  float diffuse[4];
  // ambient and diffuse
  float specular[4];
  float shininess;
  float emission[4];
  //colour indecies
} MaterialProperties;

typedef struct {
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess;
  float position[4];
  float spot_direction[3];
  float spot_exponent;
  float spot_cutoff;
  float constant_attenuation;
  float linear_attenuation;
  float quadratic_attenuation;
} LightingProperties;

typedef struct {
  char object_type[OBJECT_LOADER_MAX_SIZE];
  char model_type[OBJECT_LOADER_MAX_SIZE];
  bool use_lighting;
  bool wrap_texture;
  bool draw_self;
  bool draw_members;
  bool outline;
  bool model_by_type;
  bool multi_textures;
  float scale;
  float offset[3];
  char state[OBJECT_LOADER_MAX_SIZE];
  char select_state[OBJECT_LOADER_MAX_SIZE];
  float width;
  float height;
  int num_planes;
  int num_slicings;
  int slices_per_slicing;
  float texture_scale;
  float detail;
  MaterialProperties *material_properties;
  LightingProperties *lighting_properties;
} ObjectProperties;

class ObjectLoader {
public:
  ObjectLoader();
  ~ObjectLoader();
  void init();
  void shutdown();
  void readFiles(const std::string &);
  int readRecord(FILE *, ObjectProperties*);
  ObjectProperties *getObjectProperties(const std::string &object_type) { return _object_properties[object_type]; }

protected:
  std::map<std::string, ObjectProperties*> _object_properties;
  bool _initialised;
};

} /* namespace Sear */
#endif /* SEAR_OBJECTLOADER_H */
