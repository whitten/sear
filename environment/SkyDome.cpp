// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sage/sage.h>
#include <sage/GL.h>
#include <math.h>
#include <iostream>
#include "src/Render.h"
#include "src/System.h"
#include "src/Graphics.h"
#include "src/Calendar.h"

#include "renderers/RenderSystem.h"

#define SQR(X) ((X))

#include "SkyDome.h"
                                                                          
#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

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

#define DIST 100.0f

#define QUAD_COORDS { \
	-DIST,  DIST, 1.0f, \
	 DIST,  DIST, 1.0f, \
	 DIST, -DIST, 1.0f, \
	-DIST, -DIST, 1.0f}

#define QUAD_TEX { \
	0, 0, \
	5, 0, \
	5, 5, \
	0, 5}

namespace Sear {

float SkyDome::m_box[] = VERTEX_COORDS;
float SkyDome::m_quad_v[] = QUAD_COORDS;
float SkyDome::m_quad_t[] = QUAD_TEX;

SkyDome::SkyDome(float radius, int levels, int segments) :
    m_verts(NULL), m_texCoords(NULL),
    m_vb_verts(0), m_vb_texCoords(0)
{
    m_size = segments * (levels + 1); // extra level for the skirt
    // disable for now
    sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT] = false;
    domeInit(radius, levels, segments);
}

SkyDome::~SkyDome()
{
    delete [] m_verts;
    delete [] m_texCoords;
}

float* SkyDome::genLevelVerts(float a2, float a22, float radius, int segments, float* verts)
{
    float segmentInc = (2.0f * M_PI) / segments;
    
    for (int j = 0; j < segments; ++j) {
      float a1 = ((float)j * segmentInc);
      float a11 = ((float)(j + 1) * segmentInc);

      float x,y,z;
      
      x = sin(a1) * sin(a2) * SQR(radius);
      y = cos(a1) * sin(a2) * SQR(radius);
      z = cos(a2) * radius;

      (*verts++) = x;
      (*verts++) = y;
      (*verts++) = z;
      
      x = sin(a11) * sin(a2) * SQR(radius);
      y = cos(a11) * sin(a2) * SQR(radius);
      z = cos(a2) * radius;

      (*verts++) = x;
      (*verts++) = y;
      (*verts++) = z;
      
      x = sin(a11) * sin(a22) * SQR(radius);
      y = cos(a11) * sin(a22) * SQR(radius);
      z = cos(a22) * radius;

      (*verts++) = x;
      (*verts++) = y;
      (*verts++) = z;
      
      x = sin(a1) * sin(a22) * SQR(radius);
      y = cos(a1) * sin(a22) * SQR(radius);
      z = cos(a22) * radius;

      (*verts++) = x;
      (*verts++) = y;
      (*verts++) = z;
    }
    
    return verts;
}

float *SkyDome::genVerts(float radius, int levels, int segments)
{
  float *verts = new float[m_size * 3 * 4];
  float levelInc = M_PI / (float)levels / 2.0f;
    
  float* vptr = genLevelVerts((M_PI * 3) / 4, M_PI / 2, radius, segments, verts);

  for (int i = 0; i < levels; ++i) {
    float a2 = ((float)i * levelInc);
    float a22 = ((float)(i + 1) * levelInc);
    vptr = genLevelVerts(a2, a22, radius, segments, vptr);
  }
  
  return verts;
}
 
float *SkyDome::genTexCoords(float radius, int levels, int segments)
{
  float *tex = new float[m_size * 2 * 4];
  int tex_counter = -1;

 // tex coords for the skirt
  for (int k = 0; k < (segments * 8); ++k) {
    tex[++tex_counter] = 0.0f;
  }

  for (int i = 0; i < levels; ++i) {

    float v1 = (float)(levels - i)/(float)(levels);
    float v2 = (float)(levels - (i + 1))/(float)(levels);
    for (int j = 0; j < segments; ++j) {
      tex[++tex_counter] = 0.0f;
      tex[++tex_counter] = v1;
      tex[++tex_counter] = 0.0f;
      tex[++tex_counter] = v1;
      tex[++tex_counter] = 0.0f;
      tex[++tex_counter] = v2;
      tex[++tex_counter] = 0.0f;
      tex[++tex_counter] = v2;
    }
  }
  return tex;
}

void SkyDome::domeInit(float radius, int levels, int segments) {
  // Get texture handles
  m_textures[0] = RenderSystem::getInstance().requestTexture("atmosphere");
  m_textures[1] = RenderSystem::getInstance().requestTexture("cloud_layer_1");
  m_textures[2] = RenderSystem::getInstance().requestTexture("cloud_layer_2");

  m_verts = genVerts(radius, levels, segments);  
  m_texCoords = genTexCoords(radius, levels, segments);  

  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glGenBuffersARB(1, &m_vb_verts);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_verts);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_size * 3 * 4 * sizeof(float), m_verts, GL_STATIC_DRAW_ARB);
    delete [] m_verts;
    m_verts = NULL;

    glGenBuffersARB(1, &m_vb_texCoords);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texCoords);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_size * 2 * 4 * sizeof(float), m_texCoords, GL_STATIC_DRAW_ARB);
    delete [] m_texCoords;
    m_texCoords = NULL;
    
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
}

//static int counter = 0;

void SkyDome::render()
{
  glColor3f(1.0f, 1.0f, 1.0f);
//  ++counter;
//  #define INCR 24000
//  counter = counter % INCR;
//  float val = (float)(counter) / (float)INCR;
  Calendar *cal = System::instance()->getCalendar();
  float val = cal->getHours();
  val *= (float)cal->getMinutesPerHour();
  val += (float)cal->getMinutes();
  val *= (float)cal->getSecondsPerMinute();
  val += (float)cal->getSeconds();

  val /= (float)(cal->getSecondsPerMinute() * cal->getMinutesPerHour() * cal->getHoursPerDay());

  glEnableClientState(GL_VERTEX_ARRAY);
 
  // Select atmosphere texture
  RenderSystem::getInstance().switchTexture(m_textures[0]);

  // Translate texture for time of day 
  // and simple cloud animation
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
    glLoadIdentity();
    glTranslatef(val, 0.0f,0.0f);
  glMatrixMode(GL_MODELVIEW);

  // Quick hack so we dont call readpixels every frame
  static int delay = 20; // set so we do this in the first frame
  if (++delay >= 20) {
    glTexCoord2i(0, 0);
    glVertexPointer(3, GL_FLOAT, 0, &m_box[0]);
    glDrawArrays(GL_QUADS, 0, sizeof(m_box) / 4);
 
    GLfloat i[4];
    glReadPixels(0,0,1,1,GL_RGBA,GL_FLOAT,&i);
    glFogfv(GL_FOG_COLOR,i);
    delay = 0;
  }

  // Set the dome vetices
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { 
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_verts);
  }
  glVertexPointer(3, GL_FLOAT, 0, m_verts);

glEnable(GL_BLEND);

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  // Setup dome texture coords
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { 
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texCoords);
  }
  glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords);
  // Renderdome
  glDrawArrays(GL_QUADS, 0, m_size * 4);

glDisable(GL_BLEND);

  // Render Cloud layer one
  glEnable(GL_BLEND);
  RenderSystem::getInstance().switchTexture(m_textures[1]);
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { 
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }

  // Render large polygon for cloud layer
  // TODO split into smaller polys
  // turn down edges so its not so flat looking
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { 
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
  
  glVertexPointer(3, GL_FLOAT, 0, &m_quad_v[0]);
  glTexCoordPointer(2, GL_FLOAT, 0, &m_quad_t[0]);
  glDrawArrays(GL_QUADS, 0, 4);

  // Translate further so cloud layers move
  // at different speeds
  glMatrixMode(GL_TEXTURE);
    glTranslatef(val, 0.0f,0.0f);
  glMatrixMode(GL_MODELVIEW);

  // Render cloud layer two
  RenderSystem::getInstance().switchTexture(m_textures[2]);

  glVertexPointer(3, GL_FLOAT, 0, &m_quad_v[0]);
  glTexCoordPointer(2, GL_FLOAT, 0, &m_quad_t[0]);
  glDrawArrays(GL_QUADS, 0, 4);

  // reset texture matrix
  glMatrixMode(GL_TEXTURE);
    glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  // Reset states
  glDisable(GL_BLEND);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  // Disable vertex buffer objects
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { 
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
}

} /* namespace Sear */
