// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "common/Utility.h"

#include "src/System.h"
#include "src/Render.h"
#include "src/Camera.h"
#include "src/Character.h"

#include "Slice.h"

/**
 * TODO
 * Fix brightness balance due to blending
 * Check lighting is okay
 * Add option for a vertical slicing for viewing from above
 */ 

namespace Sear {


	
  
Slice::Slice(Render *render) : Model(render),
  _use_textures(true),
  slicings(NULL),
  _trunk_model(NULL)
{}

Slice::~Slice() {

}
  
bool Slice::init(const std::string &type, float width, float height, Model *trunk_model, unsigned int num_slicings, unsigned int slices_per_slicing) {
  _type = type;
  _trunk_model = trunk_model;
  _num_slicings = num_slicings;
  _slices_per_slicing = slices_per_slicing;
  slicings = (Slicing**)malloc(_num_slicings * sizeof(Slicing*));
  memset(slicings, 0, _num_slicings * sizeof(Slicing*));
  for (unsigned int i = 0; i < _num_slicings; i++) {
    Slicing *slicing = slicings[i] = new Slicing();
    float angle = ((1 * WFMath::Pi) / (_num_slicings)) * i;
    for (unsigned int j = 0; j < _slices_per_slicing; j++) {
      int m = j - (_slices_per_slicing / 2);
      float mod = (width / _slices_per_slicing) * m;
      float x = (width / 2) * sin(angle);
      float y = ((width / 2)) * cos(angle);
      float mod_x = sin(angle + (WFMath::Pi / 2.0f));
      float mod_y = cos(angle + (WFMath::Pi / 2.0f));
      float x_1 = x + (float) mod * mod_x;
      float x_2 = -x + (float) mod * mod_x;
      float y_1 = y + (float) mod * mod_y;
      float y_2 = -y + (float) mod * mod_y;
      ASlice *s = new ASlice();
      // Calc Vertices
      s->vertex_data[0][0] = x_1; s->vertex_data[0][1] = y_1; s->vertex_data[0][2] = 0.0f;
      s->vertex_data[1][0] = x_2; s->vertex_data[1][1] = y_2; s->vertex_data[1][2] = 0.0f;
      s->vertex_data[2][0] = x_2; s->vertex_data[2][1] = y_2; s->vertex_data[2][2] = height;
      s->vertex_data[3][0] = x_1; s->vertex_data[3][1] = y_1; s->vertex_data[3][2] = height;

      // Calc normals
      float v[3][3];
      v[0][0] = s->vertex_data[0][0]; v[0][1] = s->vertex_data[0][1]; v[0][2] = s->vertex_data[0][2];
      v[1][0] = s->vertex_data[1][0]; v[1][1] = s->vertex_data[1][1]; v[1][2] = s->vertex_data[1][2];
      v[2][0] = s->vertex_data[2][0]; v[2][1] = s->vertex_data[2][1]; v[2][2] = s->vertex_data[2][2];
      float out [3];
      calcNormal(v, out);
      s->normal_data[0][0] = out[0]; s->normal_data[0][1] = out[1]; s->normal_data[0][2] = out[2];
      s->normal_data[1][0] = out[0]; s->normal_data[1][1] = out[1]; s->normal_data[1][2] = out[2];
      s->normal_data[2][0] = out[0]; s->normal_data[2][1] = out[1]; s->normal_data[2][2] = out[2];

      // Calc Texture Coords 
      s->texture_data[0][0] = 0.0f; s->texture_data[0][1] = 0.0f;
      s->texture_data[1][0] = 1.0f; s->texture_data[1][1] = 0.0f;
      s->texture_data[2][0] = 1.0f; s->texture_data[2][1] = 1.0f;
      s->texture_data[3][0] = 0.0f; s->texture_data[3][1] = 1.0f;
      
      slicing->push_back(s);
    } 
  }

  
  return true;
}

void Slice::shutdown() {
  if (slicings) {
//    for (unsigned int i = 0; i < _num_slicings; i++) {
//    }	    
    free(slicings);
    slicings = NULL;
  }
}

void Slice::render(bool select_mode) {
  if (!_render) return;
  if (_trunk_model) _trunk_model->render(select_mode);
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
//  if (select_mode) {
//    _render->switchTexture(_render->requestMipMapMask("slice", _type, true));
//  } else {
//    _render->switchTexture(_render->requestMipMap("slice", _type, true));
//  }
//  WFMath::Quaternion q = System::instance()->getGraphics()->getCameraOrientation();
  float camera_angle = System::instance()->getGraphics()->getCamera()->getRotation();
  Character *c = System::instance()->getCharacter();
  if (c) camera_angle += c->getAngle();
  //camera_angle += WFMath::Pi / 2;
  // Keeps camera angle with 0 -> 2 Pi range
  while (camera_angle < 0.0f) camera_angle += WFMath::Pi * 2;
  while (camera_angle > WFMath::Pi * 2) camera_angle -= WFMath::Pi * 2;
  
  unsigned int index_1, index_2;
  float angle = WFMath::Pi / _num_slicings;
  float transparency = 1.0f;
  for (unsigned int i = 0; i < 2 * _num_slicings; i++) {
    if (camera_angle >= i * angle && camera_angle <= (i + 1) * angle) {
      index_1 = i;
      index_2 = i + 1;
      transparency = (camera_angle - (i * angle)) / angle;
//      if (transparency < 0.5f) transparency = 1.0f - transparency;
    }
  }
  while (index_1 >= _num_slicings) index_1 -= _num_slicings;
  while (index_2 >= _num_slicings) index_2 -= _num_slicings;
  while (index_1 < 0) index_1 += _num_slicings;
  while (index_2 < 0) index_2 += _num_slicings;
  //cout << index_1 << " " << index_2 << " " << transparency << endl; 
  _render->setColour(1.0f, 1.0f, 1.0f, 1.0f -  transparency);
  int i = 0;
  int index;
//  if (transparency < 0.5) index = index_1;
//  else index = index_2;
  index = index_1;
  for (Slicing::const_iterator I = slicings[index]->begin(); I != slicings[index]->end(); I++, i++) {
    ASlice *slice = *I;
    if (select_mode) {
      _render->switchTexture(_render->requestMipMapMask("slice", _type + "_" + string_fmt(index) + "_" + string_fmt(i), true));
    } else {
      _render->switchTexture(_render->requestMipMap("slice", _type + "_" + string_fmt(index) + "_" + string_fmt(i), true));
    }
    _render->renderArrays(Graphics::RES_QUADS, 0, 4, &slice->vertex_data[0][0], &slice->texture_data[0][0], &slice->normal_data[0][0]);
  }
//  if (transparency < 0.5) index = index_2;
//  else index = index_1;
  if (transparency != 0.0f) {
    index = index_2;
    i = 0;
    _render->setColour(1.0f, 1.0f, 1.0f,  transparency);
    for (Slicing::const_iterator I = slicings[index]->begin(); I != slicings[index]->end(); I++, i++) {
      ASlice *slice = *I;
      if (select_mode) {
        _render->switchTexture(_render->requestMipMapMask("slice", _type + "_" + string_fmt(index) + "_" + string_fmt(i), true));
      } else {
        _render->switchTexture(_render->requestMipMap("slice", _type + "_" + string_fmt(index) + "_" + string_fmt(i), true));
      }
      _render->renderArrays(Graphics::RES_QUADS, 0, 4, &slice->vertex_data[0][0], &slice->texture_data[0][0], &slice->normal_data[0][0]);
    }
  }
}

} /* namespace Sear */
