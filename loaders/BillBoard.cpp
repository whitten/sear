// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#include "BillBoard.h"
#include "../renderers/GL.h"

namespace Sear {

BillBoard::BillBoard() :
	// TODO: Should be set to false UNLESS a texture can be loaded
  _use_textures(true)
{}

BillBoard::~BillBoard() {}
  
bool BillBoard::init(const std::string &type, float _width, float _height) {
  _type = type;
  if (!_height) _height = 2.0f;
  if (!_width) _width = 2.0f;
  // Calculate billboard data
  float width_by_2 = _width / 2.0f;
  _vertex_data[0][0] = -width_by_2; _vertex_data[0][1] = 0.0f; _vertex_data[0][2] = 0.0f;
  _vertex_data[1][0] = -width_by_2; _vertex_data[1][1] = 0.0f; _vertex_data[1][2] = _height;
  _vertex_data[2][0] =  width_by_2; _vertex_data[2][1] = 0.0f; _vertex_data[2][2] = _height;
  _vertex_data[3][0] =  width_by_2; _vertex_data[3][1] = 0.0f; _vertex_data[3][2] = 0.0f;

  _texture_data[0][0] = 0.0f; _texture_data[0][1] = 0.0f;
  _texture_data[1][0] = 0.0f; _texture_data[1][1] = 1.0f;
  _texture_data[2][0] = 1.0f; _texture_data[2][1] = 1.0f;
  _texture_data[3][0] = 1.0f; _texture_data[3][1] = 0.0f;

  _normal_data[0][1] = 0.0f; _normal_data[0][2] = -1.0f; _normal_data[0][2] = 0.0f;
  _normal_data[1][1] = 0.0f; _normal_data[1][2] = -1.0f; _normal_data[1][2] = 0.0f;
  _normal_data[2][1] = 0.0f; _normal_data[2][2] = -1.0f; _normal_data[2][2] = 0.0f;
  _normal_data[3][1] = 0.0f; _normal_data[3][2] = -1.0f; _normal_data[3][2] = 0.0f;
  return true;
}

void BillBoard::shutdown() {
}

void BillBoard::render(bool select_mode) {
  if (select_mode) {	
    GL::instance()->switchTexture(GL::instance()->requestTextureMask(_type));
  } else {
    GL::instance()->switchTexture(GL::instance()->requestTexture(_type));
  }
  GL::instance()->renderArrays(Models::QUADS, _num_points, &_vertex_data[0][0], &_texture_data[0][0], NULL);
}

} /* namespace Sear */
