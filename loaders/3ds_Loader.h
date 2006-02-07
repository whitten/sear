// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: 3ds_Loader.h,v 1.9 2006-02-07 11:31:03 simon Exp $

#ifndef SEAR_3DS_LOADER_H
#define SEAR_3DS_LOADER_H 1

#include "ModelLoader.h"

namespace Sear {

// Forward Declarations
class Model;
class ModelHandler;
	
class ThreeDS_Loader : public ModelLoader {
public:	
  ThreeDS_Loader(ModelHandler *mh);
  ~ThreeDS_Loader() {}

  SPtr<ModelRecord> loadModel(Render *render, WorldEntity *we, const std::string &model_id, varconf::Config &model_config);
	  
protected:
  static const std::string THREEDS;
};

} /* namespace Sear */

#endif /* SEAR_3DS_LOADER_H */
