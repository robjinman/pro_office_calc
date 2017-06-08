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
static Point worldPointToFloorTexel(const Point& p, const Size& texSz_wd, const Size& texSz_px) {
  double nx = p.x / texSz_wd.x;
  double ny = p.y / texSz_wd.y;
  return Point((nx - floor(nx)) * texSz_px.x, (ny - floor(ny)) * texSz_px.y);
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
// drawCeiling
//===========================================
static void drawCeiling(QPainter& painter, const Scene& scene, const Point& collisionPoint,
  int wallTop_px, int screenX_px, int screenH_px, double screenX_wd, double vWorldUnitsInPx,
  double F) {

  const Camera& cam = scene.camera;
  const QPixmap& ceilingTex = scene.textures.at("ceiling");

  for (int j = wallTop_px; j >= 0; --j) {
    double screenY_wd = (screenH_px / 2 - j) / vWorldUnitsInPx;
    double vAngle = atan(screenY_wd / F);
    double hAngle = atan(screenX_wd / F);
    double d_ = scene.wallHeight / (2.0 * tan(vAngle));
    double d = d_ / cos(hAngle);

    LineSegment ray(cam.pos, collisionPoint);

    double s = d / ray.length();
    Point p(ray.A.x + (ray.B.x - ray.A.x) * s, ray.A.y + (ray.B.y - ray.A.y) * s);

    Size texSz_px(ceilingTex.rect().width(), ceilingTex.rect().height());
    double texelInWorldUnits = scene.wallHeight / texSz_px.y;
    Size texSz_wd(texSz_px.x * texelInWorldUnits, texSz_px.y * texelInWorldUnits);
    Point texel = worldPointToFloorTexel(p, texSz_wd, texSz_px);

    QRect trgRect(screenX_px, j, 1, 1);
    QRect srcRect(floor(texel.x), floor(texel.y), 1, 1);
    painter.drawPixmap(trgRect, ceilingTex, srcRect);
  }
}

//===========================================
// drawFloor
//===========================================
static void drawFloor(QPainter& painter, const Scene& scene, const Point& collisionPoint,
  int wallBottom_px, int screenX_px, int screenH_px, double screenX_wd, double vWorldUnitsInPx,
  double F) {

  const Camera& cam = scene.camera;
  const QPixmap& floorTex = scene.textures.at("floor");

  for (int j = wallBottom_px; j < screenH_px; ++j) {
    double screenY_wd = (j - screenH_px / 2) / vWorldUnitsInPx;
    double vAngle = atan(screenY_wd / F);
    double hAngle = atan(screenX_wd / F);
    double d_ = scene.wallHeight / (2.0 * tan(vAngle));
    double d = d_ / cos(hAngle);

    LineSegment ray(cam.pos, collisionPoint);

    double s = d / ray.length();
    Point p(ray.A.x + (ray.B.x - ray.A.x) * s, ray.A.y + (ray.B.y - ray.A.y) * s);

    Size texSz_px(floorTex.rect().width(), floorTex.rect().height());
    double texelInWorldUnits = scene.wallHeight / texSz_px.y;
    Size texSz_wd(texSz_px.x * texelInWorldUnits, texSz_px.y * texelInWorldUnits);
    Point texel = worldPointToFloorTexel(p, texSz_wd, texSz_px);

    QRect trgRect(screenX_px, j, 1, 1);
    QRect srcRect(floor(texel.x), floor(texel.y), 1, 1);
    painter.drawPixmap(trgRect, floorTex, srcRect);
  }
}

//===========================================
// renderScene
//===========================================
void renderScene(QPaintDevice& target, const Scene& scene) {
  QPainter painter;
  painter.begin(&target);

  const Camera& cam = scene.camera;

  int screenW_px = target.width();
  int screenH_px = target.height();

  double hWorldUnitsInPx = screenW_px / scene.viewport.x;
  double vWorldUnitsInPx = screenH_px / scene.viewport.y;

  QRect rect(QPoint(), QSize(screenW_px, screenH_px));
  painter.fillRect(rect, QBrush(QColor(0, 0, 0)));

  double F = computeF(scene.viewport.x, cam.hFov);

  for (int i = 0; i < screenW_px; ++i) {
    int i_ = i - screenW_px / 2;
    double screenX_wd = static_cast<double>(i_) / hWorldUnitsInPx;

    CastResult result = castRay(Vec2f(F, screenX_wd), scene);
    if (result.hitSomething) {
      int sliceH_px = computeSliceHeight(F, result.distanceFromCamera, scene.wallHeight)
        * vWorldUnitsInPx;

      const QPixmap& wallTex = scene.textures.at(result.texture);

      int wallBottom_px = 0.5 * (screenH_px - sliceH_px);
      QRect trgRect(i, wallBottom_px, 1, sliceH_px);
      QRect srcRect = sampleTexture(wallTex.rect(), result.distanceAlongWall, scene.wallHeight);

      painter.drawPixmap(trgRect, wallTex, srcRect);

      drawFloor(painter, scene, result.collisionPoint, wallBottom_px + sliceH_px, i, screenH_px,
        screenX_wd, vWorldUnitsInPx, F);
      drawCeiling(painter, scene, result.collisionPoint, wallBottom_px, i, screenH_px, screenX_wd,
        vWorldUnitsInPx, F);
    }
  }

  painter.end();
}
