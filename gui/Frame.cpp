// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <sage/sage.h>
#include <sage/GL.h>

#include "gui/Frame.h"

#include "renderers/Render.h"


#include <iostream>

namespace Sear {

Frame::Frame() : m_down(false)
{
}

Frame::~Frame()
{
}

void Frame::render(Render * renderer)
{
  static const GLushort line_up[] = {1, 0, 3};
  static const GLushort line_down[] = {1, 2, 3};

  const GLshort coords[] = { m_x, m_y,
                             m_x, m_y + m_h - 1,
                             m_x + m_w - 1, m_y + m_h - 1,
                             m_x + m_w - 1, m_y };

  glVertexPointer(2, GL_SHORT, 0, coords);

  renderer->setColour(0.5f, 0.5f, 0.5f, 0.4f);
  glDrawArrays(GL_QUADS, 0, 4);

  if (m_down) {
    renderer->setColour(0.3f, 0.3f, 0.3f, 1.0f);
  } else {
    renderer->setColour(0.7f, 0.7f, 0.7f, 1.0f);
  }
  glDrawElements(GL_LINE_STRIP, 3, GL_UNSIGNED_SHORT, line_up);
  if (m_down) {
    renderer->setColour(0.7f, 0.7f, 0.7f, 1.0f);
  } else {
    renderer->setColour(0.3f, 0.3f, 0.3f, 1.0f);
  }
  glDrawElements(GL_LINE_STRIP, 3, GL_UNSIGNED_SHORT, line_down);

  glPushMatrix();
  glTranslatef(m_x, m_y, 0);
  std::set<Window *>::const_iterator I = m_children.begin();
  std::set<Window *>::const_iterator Iend = m_children.end();
  for (; I != Iend; ++I) {
    (*I)->render(renderer);
  }
  glPopMatrix();
}

} // namespace Sear
