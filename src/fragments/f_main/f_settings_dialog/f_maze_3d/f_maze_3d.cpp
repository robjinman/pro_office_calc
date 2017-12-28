#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_settings_dialog/f_maze_3d/f_maze_3d.hpp"
#include "fragments/f_main/f_settings_dialog/f_maze_3d/f_maze_3d_spec.hpp"
#include "utils.hpp"


//===========================================
// FMaze3d::FMaze3d
//===========================================
FMaze3d::FMaze3d(Fragment& parent_, FragmentData& parentData_)
  : QWidget(nullptr),
    Fragment("FMaze3d", parent_, parentData_, m_data) {

  DBG_PRINT("FMaze3d::FMaze3d\n");
}

//===========================================
// FMaze3d::initialise
//===========================================
void FMaze3d::initialise(const FragmentSpec& spec_) {
  DBG_PRINT("FMaze3d::initialise\n");
}

//===========================================
// FMaze3d::reload
//===========================================
void FMaze3d::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FMaze3d::reload\n");

  auto& parentData = parentFragData<FSettingsDialogData>();

  m_data.vbox.reset(new QVBoxLayout);
  m_data.vbox->setSpacing(0);
  m_data.vbox->setContentsMargins(0, 0, 0, 0);

  setLayout(m_data.vbox.get());

  m_origParentState.spacing = parentData.vbox->spacing();
  m_origParentState.margins = parentData.vbox->contentsMargins();

  parentData.vbox->setSpacing(0);
  parentData.vbox->setContentsMargins(0, 0, 0, 0);
  parentData.vbox->addWidget(this);

  m_data.wgtRaycast.reset(new RaycastWidget(this, *parentData.eventSystem));
  m_data.vbox->addWidget(m_data.wgtRaycast.get());

  m_data.wgtRaycast->initialise();
}

//===========================================
// FMaze3d::cleanUp
//===========================================
void FMaze3d::cleanUp() {
  DBG_PRINT("FMaze3d::cleanUp\n");

  auto& parentData = parentFragData<FSettingsDialogData>();

  parentData.vbox->setSpacing(m_origParentState.spacing);
  parentData.vbox->setContentsMargins(m_origParentState.margins);
  parentData.vbox->removeWidget(this);
}

//===========================================
// FMaze3d::~FMaze3d
//===========================================
FMaze3d::~FMaze3d() {
  DBG_PRINT("FMaze3d::~FMaze3d\n");
}
