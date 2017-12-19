#include <cmath>
#include <QMouseEvent>
#include "evasive_button.hpp"
#include "utils.hpp"


static const double FRAME_RATE = 20.0;
static const double RADIUS = 80.0;


//===========================================
// distance
//===========================================
inline static double distance(const QPoint& a, const QPoint& b) {
  return sqrt(pow(a.x() - b.x(), 2) + pow(a.y() - b.y(), 2));
}

//===========================================
// length
//===========================================
inline static double length(const QPoint& p) {
  return sqrt(p.x() * p.x() + p.y() * p.y());
}

//===========================================
// normalise
//===========================================
inline static QPointF normalise(const QPoint& p) {
  return QPointF(p) / length(p);
}

//===========================================
// clamp
//===========================================
inline static double clamp(double x, double min, double max) {
  if (x < min) {
    return min;
  }
  if (x > max) {
    return max;
  }
  return x;
}

//===========================================
// EvasiveButton::EvasiveButton
//===========================================
EvasiveButton::EvasiveButton(const QString& caption)
  : QPushButton(caption),
    m_active(false) {

  setMouseTracking(true);
  setFocusPolicy(Qt::NoFocus);

  m_timer.reset(new QTimer(this));
  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(tick()));

  reset();
}

//===========================================
// EvasiveButton::reset
//===========================================
void EvasiveButton::reset() {
  m_originalPos = pos();
}

//===========================================
// EvasiveButton::cursorInRange
//===========================================
bool EvasiveButton::cursorInRange(QPoint cursor) const {
  return distance(cursor, rect().center()) <= RADIUS;
}

//===========================================
// EvasiveButton::onMouseMove
//===========================================
void EvasiveButton::onMouseMove() {
  QPoint cursor = mapFromGlobal(QCursor::pos());

  if (cursorInRange(cursor) || pos() != m_originalPos) {
    if (!m_timer->isActive()) {
      m_timer->start(1000.0 / FRAME_RATE);
    }
  }
}

//===========================================
// EvasiveButton::tick
//===========================================
void EvasiveButton::tick() {
  QPoint cursorPos = mapFromGlobal(QCursor::pos());
  QPoint btnPos = pos();
  QPoint btnCentre = rect().center();

  // The button wants to get to m_originalPos and avoid cursorPos

  double curDist = distance(cursorPos, btnCentre);

  QPointF A;
  QPointF B;

  if (curDist >= 1.0) {
    double maxSpeed = 700.0; // Pixels per second

    QPointF v = normalise(btnCentre - cursorPos);
    double speed = maxSpeed * clamp(1.0 - (curDist / RADIUS), 0, 1);
    double delta = speed / FRAME_RATE;

    A = delta * v;
  }

  double minReturnSpeed = 100.0;
  double dist = distance(m_originalPos, btnPos);

  if (dist >= 1.0) {
    double speedAtRadius = 200.0;

    QPointF v = normalise(m_originalPos - btnPos);

    double speed = clamp(speedAtRadius * dist / RADIUS, minReturnSpeed, 100 * speedAtRadius);
    double delta = speed / FRAME_RATE;

    B = delta * v;
  }

  move(btnPos + (A + B).toPoint());

  if (distance(pos(), m_originalPos) <= minReturnSpeed / FRAME_RATE) {
    move(m_originalPos);
    m_timer->stop();
  }
}

//===========================================
// EvasiveButton::mouseMoveEvent
//===========================================
void EvasiveButton::mouseMoveEvent(QMouseEvent*) {
  onMouseMove();
}

//===========================================
// EvasiveButton::~EvasiveButton
//===========================================
EvasiveButton::~EvasiveButton() {}
