#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_settings_dialog/f_maze_3d/f_maze_3d.hpp"
#include "fragments/f_main/f_settings_dialog/f_maze_3d/f_maze_3d_spec.hpp"
#include "effects.hpp"


//===========================================
// FMaze3d::FMaze3d
//===========================================
FMaze3d::FMaze3d(Fragment& parent_, FragmentData& parentData_)
  : QWidget(nullptr),
    Fragment("FMaze3d", parent_, parentData_, m_data) {}

//===========================================
// FMaze3d::rebuild
//===========================================
void FMaze3d::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<FSettingsDialog>();
  auto& parentData = parentFragData<FSettingsDialogData>();

  setParent(&parent);
  setGeometry(parent.geometry());

  auto& spec = dynamic_cast<const FMaze3dSpec&>(spec_);

  m_data.wgtRaycast.reset(new RaycastWidget(this, *parentData.eventSystem));
  m_data.wgtRaycast->setGeometry(geometry());
  m_data.wgtRaycast->initialise();

  Fragment::rebuild(spec_);
}

//===========================================
// FMaze3d::cleanUp
//===========================================
void FMaze3d::cleanUp() {
  auto& parentData = parentFragData<FSettingsDialogData>();

  setParent(nullptr);
}
