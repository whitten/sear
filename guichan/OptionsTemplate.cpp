// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "guichan/OptionsTemplate.h"

#include "guichan/RootWidget.h"
#include "guichan/box.hpp"

#include <guichan.hpp>

#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/object_slot.h>

#include <SDL/SDL.h>

#include <iostream>

namespace Sear {

OptionsTemplate::OptionsTemplate(RootWidget * top) : gcn::Window("opt"),
                                                     m_top(top)
{
  gcn::Color base = getBaseColor();
  base.a = 128;
  setBaseColor(base);

  setOpaque(true);

  gcn::Box * vbox = new gcn::VBox(6);

  gcn::Box * hbox = new gcn::HBox(6);

  gcn::Button * b = new gcn::Button("Apply");
  b->setEventId("apply");
  b->setFocusable(false);
  b->addActionListener(this);
  hbox->pack(b);

  b = new gcn::Button("Close");
  b->setEventId("close");
  b->setFocusable(false);
  b->addActionListener(this);
  hbox->pack(b);

  vbox->pack(hbox);

  setContent(vbox);

  resizeToContent();
}

OptionsTemplate::~OptionsTemplate()
{
}

void OptionsTemplate::action(const std::string & event)
{
  if (event == "apply") {
    std::cout << "Apply changes" << std::endl << std::flush;
  } else if (event == "close") {
    m_top->closeWindow(this);
  } else {
    std::cout << "Say what?" << std::endl << std::flush;
  }
}

} // namespace Sear
