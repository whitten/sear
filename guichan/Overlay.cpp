// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Alistair Riddoch
// Copyright (C) 2007 - 2009 Simon Goodall

#include "Overlay.h"
#include "SpeechBubble.h"
#include "StatusWindow.h"
#include "TaskWindow.h"
#include "RootWidget.h"

#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include "src/System.h"
#include "src/Character.h"
#include "src/CharacterManager.h"
#include "src/WorldEntity.h"
#include "src/client.h"

#include <Eris/Avatar.h>
#include <Eris/View.h>

#include <sigc++/object_slot.h>

namespace Sear {

SPtr<Overlay> Overlay::m_instance;

Overlay::Overlay() : m_top(0), m_selfStatus(0), m_selectionStatus(0)
{

}

Overlay::~Overlay()
{
}

void Overlay::logic(RootWidget * rw)
{
  Character * activeCharacter = System::instance()->getCharacterManager()->getActiveCharacter();
  Eris::Avatar * avatar = activeCharacter ? activeCharacter->getAvatar() : 0;
  Render * render = RenderSystem::getInstance().getRenderer();
  if (avatar == 0 || avatar->getEntity() == 0) {
    if (m_top != 0) {
      // If this overlay is initialised, clean it up.
      m_top->remove(m_selfStatus.get());
      m_selfStatus.release();
      if (m_selectionStatus != 0) {
        m_top->remove(m_selectionStatus.get());
        m_selectionStatus.release();
        //m_selectionStatus = 0;
      }
      // Clean up any bubbles.
      m_top = 0;
    }
    return;
  }

  if (m_top == 0) {
    avatar->Hear.connect(sigc::mem_fun(*this, &Overlay::heard));
    m_top = rw;

    m_selfStatus = SPtr<StatusWindow>(new StatusWindow(avatar->getEntity()));
    m_top->setWindowCoords(m_selfStatus.get(), std::make_pair(render->getWindowWidth() - m_selfStatus->getWidth(), 0));
    m_top->openWindow(m_selfStatus.get());

    m_selfTask = SPtr<TaskWindow>(new TaskWindow(avatar->getEntity()));
    m_top->setWindowCoords(m_selfTask.get(), std::make_pair(render->getWindowWidth() - m_selfTask->getWidth(), m_selfStatus->getHeight()));
    m_top->openWindow(m_selfTask.get());
  }

  std::string selection_id = render->getActiveID();
  Eris::Entity * selection_ent = avatar->getView()->getEntity(selection_id);

  if (selection_ent != m_selection.get() || !m_selection) {
    if (m_selectionStatus != 0) {
      m_top->remove(m_selectionStatus.get());
      m_selectionStatus.release();
//      m_selectionStatus = 0;
    }
    m_selection = selection_ent;
    if (selection_ent != 0) {
      m_selectionStatus = SPtr<StatusWindow>(new StatusWindow(selection_ent));
      m_top->add(m_selectionStatus.get(), render->getWindowWidth() - m_selfStatus->getWidth() - 4 - m_selectionStatus->getWidth(), 0);
    }
  }

  double elapsed = System::instance()->getTimeElapsed();

  //std::set<WorldEntity *> obsoletes;
  // Get current time
  float c_time = System::instance()->getTimef();

  BubbleMap::iterator I = m_bubbles.begin();
  BubbleMap::const_iterator Iend = m_bubbles.end();
  while (I != Iend) {
    WorldEntity * we = dynamic_cast<WorldEntity *>(I->first.get());
    SPtr<SpeechBubble> sb = I->second;
    if (we == NULL) {
      m_top->remove(sb.get());
      m_bubbles.erase(I++);
      continue;
    }
    int ex = we->screenX();
    int ey = we->screenY();
    if (ey < 0 || ey >= m_top->getHeight() ||
        ex < 0 || ex >= m_top->getWidth() ||
        !we->isVisible()) {
//      obsoletes.insert(we);
      // Obsolete speech bubble?
      we->releaseScreenCoords();
      m_top->remove(sb.get());
      m_bubbles.erase(I++);
      continue;
    }


    // Remove speech bubble if they have not been updated for some amount
    // of time
    // TODO: Remove hard-codded 10 seconds
    if (c_time - sb->getLastUpdateTime() > 10.0f) {
      m_bubbles.erase(I++);
      continue;
    }

    ex = (ex - sb->getWidth() / 2);
    ey = (m_top->getHeight() - ey) - sb->getHeight();
    // sb->setX(ex + sb->m_xoff);
    sb->setY(ey);

    const int x = ex + (int)sb->m_xoff; // sb->getX();
    const int y = sb->getY();
    float xoff = sb->m_xoff;
    BubbleMap::const_iterator J = m_bubbles.begin();
    for (; J != Iend; ++J) {
      if (sb.get() == J->second.get()) { continue; }
      const int ox = J->second->getX();
      const int oy = J->second->getY();
      if (abs(y - oy) < sb->getHeight() && abs(x - ox) < sb->getWidth()) {
        if (x >= ox) {
          xoff += std::min(64.f * elapsed, (double)x - ox);
          // , (float)(m_top->getWidth() - sb->getWidth()));
        } else {
          xoff -= std::min(64.f * elapsed, (double)ox - x);
        }
      }
    }
    // Ensure the xoffset does not move the bubbles off the screen
    // Right hand side
    xoff = std::min(xoff, (float)(m_top->getWidth() - (ex + sb->getWidth())));
    // Left hand side
    xoff = std::max(xoff, (float)-ex);
    // Limit new xoffset to half bubble width
    xoff = std::min(xoff, sb->getWidth() / 2.f);
    xoff = std::max(xoff, -(sb->getWidth() / 2.f));
    sb->m_xoff = xoff;
    sb->setX(ex + (int)sb->m_xoff);

    ++I;
  }
/*
  std::set<WorldEntity *>::const_iterator K = obsoletes.begin();
  std::set<WorldEntity *>::const_iterator Kend = obsoletes.end();
  for (; K != Kend; ++K) {
    BubbleMap::iterator J = m_bubbles.find(*K);
    if (J != m_bubbles.end()) {
      WorldEntity * we = dynamic_cast<WorldEntity *>(J->first.get());
      if (we) we->releaseScreenCoords();
      SPtr<SpeechBubble> b = J->second;
      m_top->remove(b.get());
      b.release();
      m_bubbles.erase(J);
    }
  }
*/
}

void Overlay::heard(Eris::Entity * e,
                    const Atlas::Objects::Operation::RootOperation & talk)
{
  assert(m_top != 0);

  Character * activeCharacter = System::instance()->getCharacterManager()->getActiveCharacter();
  Eris::Avatar * avatar = activeCharacter ? activeCharacter->getAvatar() : 0;
  //Eris::Avatar * avatar = System::instance()->getClient()->getAvatar();
  assert(avatar != 0);
  if (e == avatar->getEntity()) {
    return;
  }

  WorldEntity * we = dynamic_cast<WorldEntity *>(e);
  if (we == NULL) {
    return;
  }
  SPtr<SpeechBubble> bubble;
  BubbleMap::const_iterator I = m_bubbles.find(we);
  if (I == m_bubbles.end()) {
    bubble = SPtr<SpeechBubble>(new SpeechBubble);
    bubble->loadImages(std::vector<std::string>());
    m_top->add(bubble.get(), 50, 50);
    m_bubbles[we] = bubble;
    we->requestScreenCoords();
  } else {
    bubble = I->second;
  }

  bubble->setLastUpdateTime(System::instance()->getTimef());

  const std::vector<Atlas::Objects::Root> & talkArgs = talk->getArgs();
  if (talkArgs.empty())
  {
    return;
  }
  const Atlas::Objects::Root & talkArg = talkArgs.front();
  if (!talkArg->hasAttr("say")) {
    printf("Error: Talk but no 'say'\n");
    return;
  }
  std::string msg = talkArg->getAttr("say").asString();
  bubble->addLine(msg);
}

void Overlay::contextCreated() {
  BubbleMap::iterator I = m_bubbles.begin();
  BubbleMap::const_iterator Iend = m_bubbles.end();
  while (I != Iend) {
    SPtr<SpeechBubble> sb = I->second;
    sb->contextCreated();
    ++I;
  }
}

void Overlay::contextDestroyed(bool check) {
  BubbleMap::iterator I = m_bubbles.begin();
  BubbleMap::const_iterator Iend = m_bubbles.end();
  while (I != Iend) {
    SPtr<SpeechBubble> sb = I->second;
    sb->contextDestroyed(check);
    ++I;
  }

}

} // namespace Sear
