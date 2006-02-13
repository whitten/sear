// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: NPlane.cpp,v 1.28 2006-02-13 22:16:09 simon Exp $

#include <iostream>

#include "common/Utility.h"

#include "renderers/RenderSystem.h"

#include "NPlane.h"
#include "StaticObject.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

NPlane::NPlane(Render *) : Model(NULL),
  m_initialised(false)
{}

NPlane::~NPlane() {
  assert(m_initialised == false);
}
  
int NPlane::init(const std::string &texture, unsigned int num_planes, float width, float height) {
  assert(m_initialised == false);

  // Store texture name and get ID numbers
  m_so = SPtrShutdown<StaticObject>(new StaticObject());
  m_so->init();
  // Set material properties
  m_so->setAmbient(1.0f, 1.0f, 1.0f, 1.0f);
  m_so->setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
  m_so->setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
  m_so->setEmission(0.0f, 0.0f, 0.0f, 0.0f);
  m_so->setShininess(50.0f);

  m_so->setTexture(0,
    RenderSystem::getInstance().requestTexture(texture),
    RenderSystem::getInstance().requestTexture(texture, true));

  m_so->setNumPoints(num_planes * 12);
  // Allocate mem
  float *vertexptr = m_so->createVertexData(3 * 12 * num_planes);
  float *normalptr = m_so->createNormalData(3 * 12 * num_planes);
  float *texptr = m_so->createTextureData(2 * 12  * num_planes);

  float rads_per_segment = WFMath::Pi / (float)num_planes;

  float in[3][3];
  float out[3];
  // Compute normals
  int vertex_counter = -1;
  int normal_counter = -1;
  int texture_counter = -1;

  for (unsigned int i = 0; i < num_planes; ++i) {
    float x = width * cos ((float)i * rads_per_segment) / 2.0f;
    float y = width * sin ((float)i * rads_per_segment) / 2.0f;

    in[0][0] = vertexptr[++vertex_counter] =  x; in[0][1] = vertexptr[++vertex_counter] =  y; in[0][2] = vertexptr[++vertex_counter] = 0.0f;
    in[1][0] = vertexptr[++vertex_counter] = -x; in[1][1] = vertexptr[++vertex_counter] = -y; in[1][2] = vertexptr[++vertex_counter] = 0.0f;
    in[2][0] = vertexptr[++vertex_counter] = -x; in[2][1] = vertexptr[++vertex_counter] = -y; in[2][2] = vertexptr[++vertex_counter] = height;

    vertexptr[++vertex_counter] = -x; vertexptr[++vertex_counter] = -y; vertexptr[++vertex_counter] = height;
    vertexptr[++vertex_counter] =  x; vertexptr[++vertex_counter] =  y; vertexptr[++vertex_counter] = height;
    vertexptr[++vertex_counter] =  x; vertexptr[++vertex_counter] =  y; vertexptr[++vertex_counter] = 0.0f;

    vertexptr[++vertex_counter] =  x; vertexptr[++vertex_counter] =  y; vertexptr[++vertex_counter] = height;
    vertexptr[++vertex_counter] = -x; vertexptr[++vertex_counter] = -y; vertexptr[++vertex_counter] = height;
    vertexptr[++vertex_counter] = -x; vertexptr[++vertex_counter] = -y; vertexptr[++vertex_counter] = 0.0f;

    vertexptr[++vertex_counter] = -x; vertexptr[++vertex_counter] = -y; vertexptr[++vertex_counter] = 0.0f;
    vertexptr[++vertex_counter] =  x; vertexptr[++vertex_counter] =  y; vertexptr[++vertex_counter] = 0.0f;
    vertexptr[++vertex_counter] =  x; vertexptr[++vertex_counter] =  y; vertexptr[++vertex_counter] = height;

    calcNormal(in, out);

    // TODO Check that these are properly calculated
    normalptr[++normal_counter] =  out[0]; normalptr[++normal_counter] =  out[1]; normalptr[++normal_counter] =  out[2];
    normalptr[++normal_counter] =  out[0]; normalptr[++normal_counter] =  out[1]; normalptr[++normal_counter] =  out[2];
    normalptr[++normal_counter] =  out[0]; normalptr[++normal_counter] =  out[1]; normalptr[++normal_counter] =  out[2];

    normalptr[++normal_counter] =  out[0]; normalptr[++normal_counter] =  out[1]; normalptr[++normal_counter] =  out[2];
    normalptr[++normal_counter] =  out[0]; normalptr[++normal_counter] =  out[1]; normalptr[++normal_counter] =  out[2];
    normalptr[++normal_counter] =  out[0]; normalptr[++normal_counter] =  out[1]; normalptr[++normal_counter] =  out[2];

    normalptr[++normal_counter] = -out[0]; normalptr[++normal_counter] = -out[1]; normalptr[++normal_counter] = -out[2];
    normalptr[++normal_counter] = -out[0]; normalptr[++normal_counter] = -out[1]; normalptr[++normal_counter] = -out[2];
    normalptr[++normal_counter] = -out[0]; normalptr[++normal_counter] = -out[1]; normalptr[++normal_counter] = -out[2];

    normalptr[++normal_counter] = -out[0]; normalptr[++normal_counter] = -out[1]; normalptr[++normal_counter] = -out[2];
    normalptr[++normal_counter] = -out[0]; normalptr[++normal_counter] = -out[1]; normalptr[++normal_counter] = -out[2];
    normalptr[++normal_counter] = -out[0]; normalptr[++normal_counter] = -out[1]; normalptr[++normal_counter] = -out[2];
   
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;

    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;

    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 1.0f;
    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 0.0f;

    texptr[++texture_counter] = 1.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 0.0f;
    texptr[++texture_counter] = 0.0f; texptr[++texture_counter] = 1.0f;
  }
  m_initialised = true;
  return 0;
}

int NPlane::shutdown() {
  assert(m_initialised == true);

  contextDestroyed(true);

  m_initialised = false;
  return 0;
}

void NPlane::contextCreated() {
  m_so->contextCreated();
}

void NPlane::contextDestroyed(bool check) {
  m_so->contextDestroyed(check);
}

void NPlane::render(bool select_mode) {
  m_so->render(select_mode);
}

} /* namespace Sear */
