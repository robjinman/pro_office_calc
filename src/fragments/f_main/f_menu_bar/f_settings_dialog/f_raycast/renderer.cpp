#include <cmath>
#include <cassert>
#include <limits>
#include <QPainter>
#include <QPaintDevice>
#include <QBrush>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"


static double computeF(double w, double fov) {
  return w / (2.0 * tan(0.5 * fov));
}

static double computeSliceHeight(double F, double d, double h) {
  return h * F / d;
}

static Point lineIntersect(const Line& l0, const Line& l1) {
  Point p;

  p.x = (l1.c - l0.c) / (l0.m - l1.m);
  p.y = l0.m * p.x + l0.c;

  return p;
}

static bool isBetween(double x, double a, double b) {
  if (a < b) {
    return x >= a && x <= b;
  }
  return x >= b && x <= a;
}

static bool lineSegmentIntersect(const LineSegment& l0, const LineSegment& l1, Point& p) {
  p = lineIntersect(l0.line(), l1.line());
  return isBetween(p.x, l0.A.x, l0.B.x) && isBetween(p.x, l1.A.x, l1.B.x);
}

static LineSegment transform(const LineSegment& lseg, const Matrix& m) {
  return LineSegment(m * lseg.A, m * lseg.B);
}

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
      assert(pt.x > 0.0);

      if (pt.x < p.x) {
        p = pt;
        hitSomething = true;
      }
    }
  }

  return hitSomething;
}

void renderScene(QPaintDevice& target, const Scene& scene) {
  QPainter painter;
  painter.begin(&target);
  painter.setRenderHint(QPainter::Antialiasing);

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
