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
  Point collisionPoint;
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
// worldCoordToFloorTexel
//===========================================
static Point worldPointToFloorTexel(const Point& p, const Size& texSzWld, const Size& texSzPx) {
  double nx = p.x / texSzWld.x;
  double ny = p.y / texSzWld.y;
  return Point((nx - floor(nx)) * texSzPx.x, (ny - floor(ny)) * texSzPx.y);
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
        result.collisionPoint = cam.matrix() * pt;
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
    int i_ = i - pxW / 2;
    double scX = static_cast<double>(i_) / hWorldUnitsInPx;

    CastResult result = castRay(Vec2f(F, scX), scene);
    if (result.hitSomething) {
      int h = computeSliceHeight(F, result.distanceFromCamera, scene.wallHeight)
        * vWorldUnitsInPx;

      const QPixmap& wallTex = scene.textures.at(result.texture);

      int y = 0.5 * (pxH - h);
      QRect trgRect(i, y, 1, h);
      QRect srcRect = sampleTexture(wallTex.rect(), result.distanceAlongWall, scene.wallHeight);

      painter.drawPixmap(trgRect, wallTex, srcRect);

      for (int j = y + h; j < pxH; ++j) {
        double scY = (j - pxH / 2) / vWorldUnitsInPx;
        double vAngle = atan(scY / F);
        double hAngle = atan(scX / F);
        double d_ = scene.wallHeight / (2.0 * tan(vAngle));
        double d = d_ / cos(hAngle);

        LineSegment ray(cam.pos, result.collisionPoint);

        double s = d / ray.length();
        Point p(ray.A.x + (ray.B.x - ray.A.x) * s, ray.A.y + (ray.B.y - ray.A.y) * s);

        const QPixmap& floorTex = scene.textures.at("floor");

        Size texSzPx(floorTex.rect().width(), floorTex.rect().height());
        double texelInWorldUnits = scene.wallHeight / texSzPx.y; // TODO
        Size texSzWld(texSzPx.x * texelInWorldUnits, texSzPx.y * texelInWorldUnits);
        Point texel = worldPointToFloorTexel(p, texSzWld, texSzPx);

        QRect trgRect(i, j, 1, 1);
        QRect srcRect(floor(texel.x), floor(texel.y), 1, 1);
        painter.drawPixmap(trgRect, floorTex, srcRect);
      }
    }
  }

  painter.end();
}
