// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SEAR_GUI_WINDOW_H
#define SEAR_GUI_WINDOW_H

#include "gui/event.h"

#include <sigc++/signal.h>

#include <SDL/SDL.h>

#include <set>

namespace Sear {

class Render;

/// Low level class defining any area of the screen in which we might be
/// interested
class Window {
protected:
  Window * m_parent;
  std::set<Window *> m_children;
  unsigned int m_eventMask;
  short m_x, m_y;
  short m_w, m_h;
  bool m_visible;

  Window();

private:
  // Private and unimplemented to prevent slicing
  Window(const Window &);
  const Window & operator=(const Window &);
public:
  virtual ~Window();

  const std::set<Window *> & getChildren() const {
    return m_children;
  }

  short x() const {
    return m_x;
  }
  
  short y() const {
    return m_y;
  }
  
  short w() const {
    return m_w;
  }
  
  short h() const {
    return m_h;
  }

  void show() {
    m_visible = true;
  }

  void hide() {
    m_visible = false;
  }

  bool visible() const {
    return m_visible;
  }
  
  void addChild(Window *);
  void removeChild(Window *);
  void setPos(int, int);
  void setSize(int, int);

  void setEvents(unsigned int ev);

  void mouseMotion(short, short, short, short);
  void mouseDown(short, short);
  void mouseUp(short, short);
  void keyPress(SDLKey, Uint16);

  virtual void render(Render *) = 0;

  SigC::Signal0<void> MouseDown;
  SigC::Signal0<void> MouseUp;
  SigC::Signal0<void> MouseEnter;
  SigC::Signal0<void> MouseLeave;
  SigC::Signal2<void, SDLKey, Uint16> KeyPress;
};

} // namespace Sear

#endif // SEAR_GUI_WINDOW_H
