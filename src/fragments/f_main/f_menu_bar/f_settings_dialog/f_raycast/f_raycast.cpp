#include <QMessageBox>
#include <QMenuBar>
#include <QPainter>
#include <QBrush>
#include <QPaintEvent>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/f_raycast.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/f_raycast_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer.hpp"


//===========================================
// FRaycast::FRaycast
//===========================================
FRaycast::FRaycast(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FRaycast", parent_, parentData_, m_data) {

  auto& parent = parentFrag<FSettingsDialog>();
  auto& parentData = parentFragData<FSettingsDialogData>();

  parentData.vbox->addWidget(this);
}

//===========================================
// FRaycast::rebuild
//===========================================
void FRaycast::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FRaycastSpec&>(spec_);

  m_gameMap.reset(new Scene("data/map.svg"));
  m_camera.reset(new Camera);

  Fragment::rebuild(spec_);
}

//===========================================
// FRaycast::cleanUp
//===========================================
void FRaycast::cleanUp() {
  // TODO
}

//===========================================
// FRaycast::paintEvent
//===========================================
void FRaycast::paintEvent(QPaintEvent* event) {
  renderScene(*this, event->rect(), *m_gameMap, *m_camera);
}

//===========================================
// FRaycast::animate
//===========================================
void FRaycast::animate() {
  update();
}
