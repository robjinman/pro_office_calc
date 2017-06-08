#include <QMessageBox>
#include <QMenuBar>
#include <QPainter>
#include <QBrush>
#include <QPaintEvent>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/f_raycast.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/f_raycast_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer.hpp"


//===========================================
// FRaycast::FRaycast
//===========================================
FRaycast::FRaycast(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FRaycast", parent_, parentData_, m_data) {

  auto& parent = parentFrag<FSettingsDialog>();
  auto& parentData = parentFragData<FSettingsDialogData>();

  parentData.vbox->addWidget(this);

  setFocus();
}

//===========================================
// FRaycast::rebuild
//===========================================
void FRaycast::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FRaycastSpec&>(spec_);

  m_scene.reset(new Scene("data/map.svg"));

  m_timer.reset(new QTimer(this));
  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(tick()));

  m_timer->start(1000 / 30);

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
void FRaycast::paintEvent(QPaintEvent*) {
  QPixmap buffer(320, 240);

  renderScene(buffer, *m_scene);

  QPainter painter;
  painter.begin(this);
  painter.drawPixmap(rect(), buffer);
  painter.end();
}

//===========================================
// FRaycast::keyPressEvent
//===========================================
void FRaycast::keyPressEvent(QKeyEvent* event) {
  m_keyStates[event->key()] = true;
}

//===========================================
// FRaycast::keyReleaseEvent
//===========================================
void FRaycast::keyReleaseEvent(QKeyEvent* event) {
  m_keyStates[event->key()] = false;
}

//===========================================
// FRaycast::tick
//===========================================
void FRaycast::tick() {
  static const double da = PI / 30;
  static const double ds = 8;

  Camera& cam = m_scene->camera;

  if (m_keyStates[Qt::Key_Left]) {
    cam.angle -= da;
  }

  if (m_keyStates[Qt::Key_Right]) {
    cam.angle += da;
  }

  if (m_keyStates[Qt::Key_Up]) {
    cam.pos.x += ds * cos(cam.angle);
    cam.pos.y += ds * sin(cam.angle);
  }

  if (m_keyStates[Qt::Key_Down]) {
    cam.pos.x -= ds * cos(cam.angle);
    cam.pos.y -= ds * sin(cam.angle);
  }

  update();
}
