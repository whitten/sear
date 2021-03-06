// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

// $Id: Cal3dModel.h,v 1.32 2007-04-22 18:28:32 simon Exp $

#ifndef SEAR_LOADERS_CAL3D_CAL3DMODEL_H
#define SEAR_LOADERS_CAL3D_CAL3DMODEL_H 1

#include <map>
#include <string>
#include <vector>
#include <memory>

#include <cal3d/cal3d.h>
#include "Cal3dCoreModel.h"
#include "Model.h"
#include "renderers/RenderTypes.h"

namespace Sear {

class DynamicObject;
class WorldEntity;

class Cal3dModel : public Sear::Model {
public:
  Cal3dModel();
  virtual ~Cal3dModel();

  int init(Cal3dCoreModel *);
  virtual int shutdown();

  virtual bool isInitialised() const { return m_initialised; }

  virtual void contextCreated();
  virtual void contextDestroyed(bool check);

  float getLodLevel() const { return m_lodLevel; }

  virtual void render(bool select_mode);
  virtual void update(float elapsedSeconds);
  void setDetailLevel(float level) { setLodLevel(level); }
  void setLodLevel(float lodLevel);

  virtual void action(const std::string &action);
  virtual void animate(const std::string &action);
  virtual RotationStyle rotationStyle() const { return ROS_NORMAL; }
 
  unsigned int getPartID(const std::string &part) { return m_core_model->m_parts[part]; }
  unsigned int getSetID(const std::string &set) { return m_core_model->m_sets[set]; }
  
  void setMaterialSet(const std::string &set) {
    setMaterialSet(m_core_model->m_sets[set]);
  }
  
  void setMaterialSet(unsigned int set);
  
  void setMaterialPartSet(const std::string &mesh, const std::string &set) {
    setMaterialPartSet(m_core_model->m_meshes[mesh], m_core_model->m_sets[set]);
  }
  void setMaterialPartSet(unsigned int part, unsigned int set);
 
  std::list<std::string> getMeshNames() const;
  std::list<std::string> &getMaterialNames() const;

  void setRotate(float r) { m_rotate = r; }
  
  virtual PosAndOrient getPositionForSubmodel(const std::string& submodelName) const;

  void addAnimation(const Cal3dCoreModel::WeightList &list);
  void removeAnimation(const Cal3dCoreModel::WeightList &list);

  void clearOutfit();
  void entityWorn(const std::string &where, WorldEntity *we);
  void entityWorn(WorldEntity *we);
  void entityRemoved(WorldEntity *we);

  virtual bool hasDynamicObjects() const { return true; }
  virtual DynamicObjectList &getDynamicObjects() { return m_dos; }

  void setState(int s) { m_state = s; }
  int getState() const { return m_state; }

  void setSelectState(int s) { m_select_state = s; }
  int getSelectState() const { return m_select_state; }

  void setUseStencil(bool b) { m_use_stencil = b; }



private:
  void renderMesh(bool bWireframe, bool bLight, bool);

  bool m_initialised;

  Cal3dCoreModel *m_core_model;
  std::auto_ptr<CalModel> m_calModel;
  float m_lodLevel;

  float m_rotate;
  std::string m_cur_anim;
  DynamicObjectList m_dos;

  std::vector<int> m_attached_meshes;

  friend class Cal3dCoreModel;


  int m_state, m_select_state;
  bool m_use_stencil;
};


} /* namespace Sear */

#endif /* SEAR_LOADERS_CAL3D_CAL3DMODEL_H */

