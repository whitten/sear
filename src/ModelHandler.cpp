// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton


#include "ModelHandler.h"
#include "ModelLoader.h"
#include "Models.h"
#include "Exception.h"
#include "WorldEntity.h"
#include "System.h"
#include "Config.h"
#include "ObjectLoader.h"
#include <Eris/TypeInfo.h>

namespace Sear {

ModelHandler::ModelHandler() :
  _model_loaders(std::map<std::string, ModelLoader*>()),
  _models(std::map<std::string, Models*>())	
{}

ModelHandler::~ModelHandler() {}

void ModelHandler::init() {

}

void ModelHandler::shutdown() {
 // Clear Up Models
}
  
Models *ModelHandler::getModel(WorldEntity *we) {
  // Check for NULL pointer	
  if (!we) throw Exception("WorldEntity is NULL");
  
  // Get model details
  // TODO: find out whether typeinfo will ever be more than two layers
  std::string type = we->getType()->getName();
  std::string parent = *we->getType()->getParentsAsSet().begin();
  std::string id = we->getID();

  // If entity already has an associated model, return it.
  if (_models[id]) return _models[id];
  
  // Get object type record
  ObjectProperties *op = NULL;
  ObjectLoader *ol = System::instance()->getObjectLoader();
  std::string model_type = "";
  std::string object_type = "";

  if (!type.empty()) {
    op = ol->getObjectProperties(type);
    object_type = type;
  }
  if (op == NULL && !parent.empty()) {
    op = ol->getObjectProperties(parent);
    object_type = parent;
  }
  if (op == NULL) {
     op = ol->getObjectProperties("default");
     object_type = "default";
  }
  model_type = op->model_type;		
  
  std::string data_source = System::instance()->getModel()->getAttribute(type);
  Models *model = NULL;
  if (_model_loaders[model_type]) model = _model_loaders[model_type]->loadModel(we, data_source);
  else ; // Ignore error 
  _models[id] = model;
  
  return model; 
}

void ModelHandler::registerModelLoader(const std::string &model_type, ModelLoader *model_loader) {
  // Check for bad values	
  if (model_type.empty()) throw Exception("No type specified");
  if (!model_loader) throw Exception("No model loader given");
  // Throw error if we already have a loader for this type
  // TODO: decide whether we should override existing loaders with new one.
  if (_model_loaders[model_type]) throw Exception("Model loader already exists for this type!");
  // If all is well, assign loader
  _model_loaders[model_type] = model_loader;
}


} /* namespace Sear */