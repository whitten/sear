// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: ModelRecord.cpp,v 1.3 2005-04-15 16:21:01 simon Exp $

#include "ModelRecord.h"
#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif


namespace Sear {
  const std::string ModelRecord::SCALE = "scale";
  const std::string ModelRecord::SCALE_BBOX = "scale_bbox";
  const std::string ModelRecord::OFFSET_X = "offset_x";
  const std::string ModelRecord::OFFSET_Y = "offset_y";
  const std::string ModelRecord::OFFSET_Z = "offset_z";
  const std::string ModelRecord::ROTATE_X = "rotate_x";
  const std::string ModelRecord::ROTATE_Y = "rotate_y";
  const std::string ModelRecord::ROTATE_Z = "rotate_z";
  const std::string ModelRecord::STATE = "state";
  const std::string ModelRecord::SELECT_STATE = "select_state";
  const std::string ModelRecord::MODEL_BY_TYPE = "model_by_type";
  const std::string ModelRecord::MODEL_LOADER = "model_loader";
  const std::string ModelRecord::OUTLINE = "outline";
  const std::string ModelRecord::ROTATION_STYLE = "rotation_style";
  const std::string ModelRecord::DATA_FILE_ID = "data_file_id";
  const std::string ModelRecord::DEFAULT_SKIN = "default_skin";

} /* namespace Sear */

