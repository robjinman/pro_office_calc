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

  m_timer->start(1000 / 60);

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
// rotateCamera
//===========================================
static void rotateCamera(Scene& scene, double da) {
  scene.camera.angle += da;
}

//===========================================
// intersectWall
//===========================================
static bool intersectWall(const Scene& scene, const Circle& circle) {
  for (auto it = scene.walls.begin(); it != scene.walls.end(); ++it) {
    if (lineSegmentCircleIntersect(circle, it->lseg)) {
      return true;
    }
  }
  return false;
}

//===========================================
// translateCamera
//===========================================
static void translateCamera(Scene& scene, double ds) {
  Camera& cam = scene.camera;

  Vec2f dv(ds * cos(cam.angle), ds * sin(cam.angle));

  double radius = scene.wallHeight / 5.0;

  Circle circle{cam.pos + dv, radius};
  LineSegment ray(cam.pos, cam.pos + dv);

  bool collision = false;
  for (auto it = scene.walls.begin(); it != scene.walls.end(); ++it) {
    const Wall& wall = *it;

    if (lineSegmentCircleIntersect(circle, wall.lseg)) {
      collision = true;

      Matrix m(-atan(wall.lseg.line().m), Vec2f());
      Vec2f dv_ = m * dv;
      dv_.y = 0;
      dv_ = m.inverse() * dv_;

      Circle circle2{cam.pos + dv_, radius};

      if (!intersectWall(scene, circle2)) {
        cam.pos = cam.pos + dv_;
        return;
      }
    }
  }

  if (!collision) {
    cam.pos = cam.pos + dv;
  }
}

//===========================================
// FRaycast::tick
//===========================================
void FRaycast::tick() {
  static const double da = PI / 50;
  static const double ds = 5;

  if (m_keyStates[Qt::Key_Left]) {
    rotateCamera(*m_scene, -da);
  }

  if (m_keyStates[Qt::Key_Right]) {
    rotateCamera(*m_scene, da);
  }

  if (m_keyStates[Qt::Key_Up]) {
    translateCamera(*m_scene, ds);
  }

  if (m_keyStates[Qt::Key_Down]) {
    translateCamera(*m_scene, -ds);
  }

  update();
}
