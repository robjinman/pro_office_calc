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


#ifdef DEBUG
namespace chrono = std::chrono;
#endif


const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 360;
const int FRAME_RATE = 60;


//===========================================
// FRaycast::FRaycast
//===========================================
FRaycast::FRaycast(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FRaycast", parent_, parentData_, m_data) {

  auto& parent = parentFrag<FSettingsDialog>();
  auto& parentData = parentFragData<FSettingsDialogData>();

  parentData.vbox->addWidget(this);
  setMouseTracking(true);

  m_defaultCursor = cursor().shape();
  m_cursorCaptured = false;

  m_buffer = QImage(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_ARGB32);

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

  m_timer->start(1000 / FRAME_RATE);

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
  m_renderer.renderScene(m_buffer, *m_scene);

  QPainter painter;
  painter.begin(this);
  painter.drawImage(rect(), m_buffer);
  painter.end();
}

//===========================================
// FRaycast::keyPressEvent
//===========================================
void FRaycast::keyPressEvent(QKeyEvent* event) {
  m_keyStates[event->key()] = true;

  if (event->key() == Qt::Key_F) {
    DBG_PRINT("Frame rate = " << m_frameRate << "\n");
  }
  else if (event->key() == Qt::Key_Escape) {
    m_cursorCaptured = false;
    setCursor(m_defaultCursor);
  }
}

//===========================================
// FRaycast::keyReleaseEvent
//===========================================
void FRaycast::keyReleaseEvent(QKeyEvent* event) {
  m_keyStates[event->key()] = false;
}

//===========================================
// FRaycast::mousePressEvent
//===========================================
void FRaycast::mousePressEvent(QMouseEvent* event) {
  m_cursorCaptured = true;
  m_cursor.x = width() / 2;
  m_cursor.y = height() / 2;

  setCursor(Qt::BlankCursor);
}

//===========================================
// FRaycast::mouseMoveEvent
//===========================================
void FRaycast::mouseMoveEvent(QMouseEvent* event) {
  m_cursor.x = event->x();
  m_cursor.y = event->y();
}

//===========================================
// rotateCamera
//===========================================
static void rotateCamera(Scene& scene, double da) {
  scene.camera->angle += da;
}

//===========================================
// intersectWall
//===========================================
static bool intersectWall(const Scene& scene, const Circle& circle) {
  for (auto it = scene.walls.begin(); it != scene.walls.end(); ++it) {
    if (lineSegmentCircleIntersect(circle, (*it)->lseg)) {
      return true;
    }
  }
  return false;
}

//===========================================
// translateCamera
//===========================================
static void translateCamera(Scene& scene, const Vec2f& dir) {
  Camera& cam = *scene.camera;

  Vec2f dv(cos(cam.angle) * dir.x - sin(cam.angle) * dir.y,
    sin(cam.angle) * dir.x + cos(cam.angle) * dir.y);

  double radius = scene.wallHeight / 5.0;

  Circle circle{cam.pos + dv, radius};
  LineSegment ray(cam.pos, cam.pos + dv);

  bool collision = false;
  for (auto it = scene.walls.begin(); it != scene.walls.end(); ++it) {
    const Wall& wall = **it;

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
#ifdef DEBUG
  if (m_frame % 10 == 0) {
    chrono::high_resolution_clock::time_point t_ = chrono::high_resolution_clock::now();
    chrono::duration<double> span = chrono::duration_cast<chrono::duration<double>>(t_ - m_t);
    m_frameRate = 10.0 / span.count();
    m_t = t_;
  }
  ++m_frame;
#endif

  Vec2f v; // The vector is in camera space
  if (m_keyStates[Qt::Key_A]) {
    v.y -= 1;
  }
  if (m_keyStates[Qt::Key_D]) {
    v.y += 1;
  }
  if (m_keyStates[Qt::Key_W] || m_keyStates[Qt::Key_Up]) {
    v.x += 1;
  }
  if (m_keyStates[Qt::Key_S] || m_keyStates[Qt::Key_Down]) {
    v.x -= 1;
  }

  if (v.x != 0 || v.y != 0) {
    double ds = 5;
    translateCamera(*m_scene, normalise(v) * ds);
  }

  if (m_keyStates[Qt::Key_Left]) {
    rotateCamera(*m_scene, -0.02 * PI);
  }
  if (m_keyStates[Qt::Key_Right]) {
    rotateCamera(*m_scene, 0.02 * PI);
  }

  if (m_cursorCaptured) {
    Point centre(width() / 2, height() / 2);

    // Y-axis is top to bottom
    Point v(m_cursor.x - centre.x, centre.y - m_cursor.y);

    QCursor::setPos(mapToGlobal(QPoint(centre.x, centre.y)));
    m_cursor = centre;

    if (fabs(v.x) > 0) {
      double da = 0.0008 * PI * v.x;
      rotateCamera(*m_scene, da);
    }

    if (fabs(v.y) > 0) {
      double da = 0.0008 * PI * v.y;

      if (fabs(m_scene->camera->vAngle + da) < fabs(DEG_TO_RAD(20))) {
        m_scene->camera->vAngle += da;
      }
    }
  }

  update();
}
