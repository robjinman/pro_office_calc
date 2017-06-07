#include <cmath>
#include <cassert>
#include <limits>
#include <QPainter>
#include <QPaintDevice>
#include <QBrush>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"


using std::string;


struct CastResult {
  bool hitSomething = false;
  double distanceFromCamera;
  double distanceAlongWall;
  string texture;
};


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
//===========================================
static CastResult castRay(Vec2f r, const Scene& scene) {
  CastResult result;
  LineSegment ray(Point(0, 0), Point(r.x * 999.9, r.y * 999.9));

  const Camera& cam = scene.camera;

  double inf = std::numeric_limits<double>::infinity();
  result.distanceFromCamera = inf;

  for (auto it = scene.walls.begin(); it != scene.walls.end(); ++it) {
    const Wall& wall = *it;
    LineSegment lseg = transform(wall.lseg, cam.matrix().inverse());

    Point pt;
    if (lineSegmentIntersect(ray, lseg, pt)) {
      // In camera space the ray will always point in positive x direction
      assert(pt.x > 0.0);

      if (pt.x < result.distanceFromCamera) {
        result.distanceFromCamera = pt.x;
        result.distanceAlongWall = distance(lseg.A, pt);
        result.hitSomething = true;
        result.texture = wall.texture;
      }
    }
  }

  return result;
}

//===========================================
// sampleTexture
//===========================================
static QRect sampleTexture(const QRect& rect, double distanceAlongWall, double wallHeight) {
  double worldUnit = static_cast<double>(rect.height()) / wallHeight;
  double texW = static_cast<double>(rect.width()) / worldUnit;

  double n = distanceAlongWall / texW;
  double x = (n - floor(n)) * texW;

  int i = x * worldUnit;
  return QRect(i, 0, 1, rect.height());
}

//===========================================
// renderScene
//===========================================
void renderScene(QPaintDevice& target, const Scene& scene) {
  QPainter painter;
  painter.begin(&target);

  const Camera& cam = scene.camera;

  int pxW = target.width();
  int pxH = target.height();

  double hWorldUnitsInPx = pxW / scene.viewport.x;
  double vWorldUnitsInPx = pxH / scene.viewport.y;

  QRect rect(QPoint(), QSize(pxW, pxH));
  painter.fillRect(rect, QBrush(QColor(0, 0, 0)));

  double F = computeF(scene.viewport.x, cam.hFov);

  for (int i = 0; i < pxW; ++i) {
    int i_ = i - 300;
    double scX = static_cast<double>(i_) / hWorldUnitsInPx;

    CastResult result = castRay(Vec2f(F, scX), scene);
    if (result.hitSomething) {
      double h = computeSliceHeight(F, result.distanceFromCamera, scene.wallHeight)
        * vWorldUnitsInPx;

      const QPixmap& tex = scene.textures.at(result.texture);

      QRect trgRect(i, 0.5 * (pxH - h), 1, h);
      QRect srcRect = sampleTexture(tex.rect(), result.distanceAlongWall, scene.wallHeight);

      painter.drawPixmap(trgRect, tex, srcRect);
    }
  }

  painter.end();
}
