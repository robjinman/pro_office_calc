#include <cmath>
#include <cassert>
#include <limits>
#include <QPainter>
#include <QPaintDevice>
#include <QBrush>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"


//===========================================
// computeF
//===========================================
static double computeF(double w, double fov) {
  return w / (2.0 * tan(0.5 * fov));
}

//===========================================
// computeSliceHeight
//===========================================
static double computeSliceHeight(double F, double d, double h) {
  return h * F / d;
}

//===========================================
// castRay
//
// This is performed in camera space, hence geometry is transformed by the
// inverse of the camera's transformation matrix prior to intersection test.
//===========================================
static bool castRay(Vec2f r, const Scene& scene, Point& p) {
  LineSegment ray(Point(0, 0), Point(r.x * 999.9, r.y * 999.9));

  const Camera& cam = *scene.camera;

  double inf = std::numeric_limits<double>::infinity();
  p.x = inf;
  p.y = inf;
  bool hitSomething = false;

  for (auto it = scene.walls.begin(); it != scene.walls.end(); ++it) {
    LineSegment lseg = transform(**it, cam.matrix().inverse());

    Point pt;
    if (lineSegmentIntersect(ray, lseg, pt)) {
      // In camera space the ray will always point in positive x direction
      assert(pt.x > 0.0);

      if (pt.x < p.x) {
        p = pt;
        hitSomething = true;
      }
    }
  }

  return hitSomething;
}

//===========================================
// renderScene
//===========================================
void renderScene(QPaintDevice& target, const Scene& scene) {
  QPainter painter;
  painter.begin(&target);

  Camera& cam = *scene.camera;

  int pxW = target.width();
  int pxH = target.height();

  double hWorldUnitsInPx = pxW / scene.viewport.x;
  double vWorldUnitsInPx = pxH / scene.viewport.y;

  QRect rect(QPoint(), QSize(pxW, pxH));
  painter.fillRect(rect, QBrush(QColor(0, 0, 0)));

  painter.setPen(QPen(QColor(164, 132, 164)));

  double F = computeF(scene.viewport.x, cam.hFov);

  for (int i = 0; i < pxW; ++i) {
    int i_ = pxW / 2 - i;
    double scX = static_cast<double>(i_) / hWorldUnitsInPx;

    Point p;
    if (castRay(Vec2f(F, scX), scene, p)) {
      double h = computeSliceHeight(F, p.x, scene.wallHeight) * vWorldUnitsInPx;
      painter.drawLine(i, 0.5 * (pxH - h), i, 0.5 * (pxH + h));
    }
  }

  painter.end();
}
