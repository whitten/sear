// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

// $Id: ObjectRecord.h,v 1.15 2007-04-01 19:00:21 simon Exp $

#ifndef SEAR_LOADERS_OBJECTRECORD_H
#define SEAR_LOADERS_OBJECTRECORD_H 1

#include <string>
#include <list>
#include <map>
#include <Eris/EntityRef.h>
#include <wfmath/axisbox.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>

namespace Atlas
{
namespace Message
{
class Element;
typedef std::map<std::string, Element> MapType;
}
}

namespace Sear {

class WorldEntity;

class ObjectRecord {
public:
  typedef std::list<std::string> ModelList;
  
  ObjectRecord();    
  ~ObjectRecord();

  void action(const std::string &action); 
  void animate(const std::string &action); 
 
  void clearOutfit();
  void entityWorn(const std::string &where, WorldEntity *we);
  void entityWorn(WorldEntity *we);
  void entityRemoved(WorldEntity *we);
 
  std::string name;
  std::string id;
  std::string type;

  typedef enum {
    QUEUE_low = 0,
    QUEUE_medium,
    QUEUE_high,
    QUEUE_LAST
  } QueueType;

  ModelList quality_queue[QUEUE_LAST];

  bool draw_self;
  bool draw_members;
  bool draw_attached;
  WFMath::AxisBox<3> bbox;
  WFMath::Point<3> position;
  WFMath::Quaternion orient;

  Eris::EntityRef entity;
};

} /* namespace Sear */

#endif /* SEAR_OBJECTRECORD_H */
