// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef SEAR_GUICHAN_ALERT_H
#define SEAR_GUICHAN_ALERT_H

#include <guichan/widgets/window.hpp>

#include <sigc++/object.h>

namespace gcn {

class Button;
class Container;

} // namespace gcn

namespace Sear {

class ActionListenerSigC;

class Alert : virtual public SigC::Object, public gcn::Window {
protected:
  gcn::Button * m_okButton;

  ActionListenerSigC * m_buttonListener;
public:
  explicit Alert(gcn::Container *, const std::string &);
  virtual ~Alert();

  void actionPressed(std::string);
};

} // namespace Sear

#endif // SEAR_GUICHAN_ALERT_H