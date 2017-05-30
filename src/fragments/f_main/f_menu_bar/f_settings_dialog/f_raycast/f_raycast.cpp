#include <QMessageBox>
#include <QMenuBar>
#include <QPainter>
#include <QBrush>
#include <QPaintEvent>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/f_raycast.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/f_raycast_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/game_map.hpp"


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

  GameMap map("data/map.svg");

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
  QPainter painter;
  painter.begin(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.fillRect(event->rect(), QBrush(QColor(64, 32, 64)));

  painter.end();
}

//===========================================
// FRaycast::animate
//===========================================
void FRaycast::animate() {
  update();
}
