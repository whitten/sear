// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef MODELSTRUCT_H
#define MODELSTRUCT_H 1

#include <wfmath/axisbox.h>

namespace Sear {

typedef struct {
  const char *file_name;
  const char *type;
  const char *parent;
  float scale;
  float width;
  float height;
  bool wrap_texture;
  int num_planes;
  WFMath::AxisBox<3> bbox;
  bool hasBBox;
} ModelStruct;

} /* namespace Sear */
	
#endif /* MODELSTRUCT_H */