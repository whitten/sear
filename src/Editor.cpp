#include "Editor.h"
#include "System.h"
#include "Graphics.h"
#include "Render.h"

#include <Eris/Entity.h>
#include <Eris/World.h>
#include <Eris/Connection.h>

#include <wfmath/atlasconv.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Delete.h>

namespace Sear {

static const std::string EDIT_MOVE_X = "edit_move_local_x";
static const std::string EDIT_MOVE_Y = "edit_move_local_y";
static const std::string EDIT_MOVE_Z = "edit_move_local_z";

static const std::string EDIT_MOVE_LOC_X = "edit_move_x";
static const std::string EDIT_MOVE_LOC_Y = "edit_move_y";
static const std::string EDIT_MOVE_LOC_Z = "edit_move_z";

static const std::string EDIT_ROTATE_Z = "edit_rotate_z";

static const std::string EDIT_DELETE = "edit_delete";

void Editor::registerCommands(Console *console)
{
    console->registerCommand(EDIT_MOVE_X, this);
    console->registerCommand(EDIT_MOVE_Y, this);
    console->registerCommand(EDIT_MOVE_Z, this);
    
    console->registerCommand(EDIT_MOVE_LOC_X, this);
    console->registerCommand(EDIT_MOVE_LOC_Y, this);
    console->registerCommand(EDIT_MOVE_LOC_Z, this);
    
    console->registerCommand(EDIT_ROTATE_Z, this);
    console->registerCommand(EDIT_DELETE, this);
}

void Editor::runCommand(const std::string &command, const std::string &args)
{
    std::string target = System::instance()->getGraphics()->getRender()->getActiveID();
  
    if (command == EDIT_MOVE_X) {
        WFMath::Vector<3> delta(::strtod(args.c_str(), NULL),0,0);
        translate(target, delta, true);
    }
    
    if (command == EDIT_MOVE_Y) {
        WFMath::Vector<3> delta(0,::strtod(args.c_str(), NULL),0);
        translate(target, delta, true);
    }
    
    if (command == EDIT_MOVE_Z) {
        WFMath::Vector<3> delta(0,0,::strtod(args.c_str(), NULL));
        translate(target, delta, true);
    }

    if (command == EDIT_MOVE_LOC_X) {
        WFMath::Vector<3> delta(::strtod(args.c_str(), NULL),0,0);
        translate(target, delta, false);
    }
    
    if (command == EDIT_MOVE_LOC_Y) {
        WFMath::Vector<3> delta(0,::strtod(args.c_str(), NULL),0);
        translate(target, delta, false);
    }
    
    if (command == EDIT_MOVE_LOC_Z) {
        WFMath::Vector<3> delta(0,0,::strtod(args.c_str(), NULL));
        translate(target, delta, false);
    }
    
    if (command == EDIT_ROTATE_Z) {
        rotateZ(target, ::strtod(args.c_str(), NULL));
    }
    
    if (command == EDIT_DELETE)
        deleteEntity(target);
}

void Editor::translate(const std::string& target, WFMath::Vector<3> delta, bool local)
{
    Eris::EntityPtr te = Eris::World::Instance()->lookup(target);
    if (!te) return; // odd
    
    Atlas::Objects::Operation::Move move;
    move.setFrom(getAvatar()->getID());
    move.setTo(te->getID());
    
    if (local) {
        delta = te->toParentCoords(delta);
        std::cout << "making vector local" << delta << std::endl;
    }
    
    Atlas::Message::Element::MapType msg;
    WFMath::Point<3> pos = te->getPosition() + delta;
    msg["pos"] = pos.toAtlas();
    msg["id"] = te->getID();
    msg["loc"] = te->getContainer()->getID();
    move.setArgs(Atlas::Message::Element::ListType(1, msg));
      
    getAvatar()->getWorld()->getConnection()->send(move);
}

void Editor::rotateZ(const std::string& target, double delta)
{
    Eris::EntityPtr te = Eris::World::Instance()->lookup(target);
    if (!te) return; // odd
    
    Atlas::Objects::Operation::Move move;
    move.setFrom(getAvatar()->getID());
    move.setTo(te->getID());
    
    Atlas::Message::Element::MapType msg;
    WFMath::Quaternion q = te->getOrientation() * WFMath::Quaternion(WFMath::Vector<3>(0.0, 0.0, 1.0), delta);
    
    msg["orientation"] = q.toAtlas();
    msg["pos"] = te->getPosition().toAtlas();    
    msg["loc"] = te->getContainer()->getID();
    msg["id"] = te->getID();
    move.setArgs(Atlas::Message::Element::ListType(1, msg));
      
    getAvatar()->getWorld()->getConnection()->send(move);
}

void Editor::deleteEntity(const std::string& target)
{
    Eris::EntityPtr te = Eris::World::Instance()->lookup(target);
    if (!te) return; // odd
    
    Atlas::Objects::Operation::Delete del;
    del.setFrom(getAvatar()->getID());
    del.setTo(te->getID());
    
    Atlas::Message::Element::MapType msg;
    msg["id"] = te->getID();
    del.setArgs(Atlas::Message::Element::ListType(1, msg));
      
    getAvatar()->getWorld()->getConnection()->send(del);
}

Eris::Avatar* Editor::getAvatar() const
{
    return Eris::World::Instance()->getPrimaryAvatar();
}

} // of namespace Sear
