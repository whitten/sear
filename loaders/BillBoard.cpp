// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: BillBoard.cpp,v 1.12 2002-09-07 23:27:05 simon Exp $

#include "src/System.h"
#include "src/Graphics.h"
#include "src/Render.h"

#include "BillBoard.h"

namespace Sear {

BillBoard::BillBoard(Render *render) : Model(render),
	// TODO: Should be set to false UNLESS a texture can be loaded
  _use_textures(true),
  _initialised(false)
{}

BillBoard::~BillBoard() {
  if (_initialised) shutdown();
}
  
bool BillBoard::init(const std::string &type, float _width, float _height) {
  if (_initialised) shutdown();
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
  _initialised = true;
  return true;
}

void BillBoard::shutdown() {
  _initialised = false;
}

void BillBoard::render(bool select_mode) {
  static float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  _render->setMaterial(&ambient[0], &diffuse[0], &specular[0], 50.0f, NULL);
  if (select_mode) {	
    _render->switchTexture(_render->requestMipMapMask("billboard", _type, true));
  } else {
    _render->switchTexture(_render->requestMipMap("billboard", _type, true));
  }
  _render->renderArrays(Graphics::RES_QUADS, 0, _num_points, &_vertex_data[0][0], &_texture_data[0][0], NULL);
}

} /* namespace Sear */
