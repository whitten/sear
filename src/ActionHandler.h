// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: ActionHandler.h,v 1.7 2005-06-18 21:08:33 simon Exp $

#ifndef SEAR_ACTIONHANDLER_H
#define SEAR_ACTIONHANDLER_H 1

#include "System.h"

#include <map>
#include <string>

#include <sigc++/object_slot.h>

#include "interfaces/ConsoleObject.h"

// Forward declaration
namespace varconf {
  class Config;
}

namespace Sear {

//Forward declaration
class Console;
class WorldEntity;

class ActionHandler : public ConsoleObject, public SigC::Object {
public:
  ActionHandler(System *system);
  ~ActionHandler();

  void init();
  void shutdown();

  void loadConfiguration(const std::string &filename);
  
  void handleAction(const std::string &action, WorldEntity *entity);

  void registerCommands(Console *console);
  void runCommand(const std::string &command, const std::string &args);

private:
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);
  
  typedef struct {
    std::string action;
    std::string script;
    bool entity_based;
  } ActionStruct;
 
  typedef std::map<std::string, ActionStruct*> ActionMap; 
  ActionMap action_map;

  System *m_system;

  bool m_initialised;
  
};
  
} /* namespace Sear */
#endif /* SEAR_ACTIONHANDLER_H */
