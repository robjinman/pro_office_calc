#include <cmath>
#include <cassert>
#include <limits>
#include <list>
#include <QPainter>
#include <QPaintDevice>
#include <QBrush>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"


using std::string;
using std::list;
using std::array;


static const double ATAN_MIN = -10.0;
static const double ATAN_MAX = 10.0;


struct Collision {
  Point collisionPoint;
  double distanceFromCamera;
  double distanceAlongTarget;
  double sliceBottom_wd;
  double sliceTop_wd;
  double projSliceBottom_wd;
  double projSliceTop_wd;
};

struct WallCollision {
  const Wall* wall;
  Collision collision;
};

struct SpriteCollision {
  const Sprite* sprite;
  Collision collision;
};

struct CastResult {
  bool hitWall = false;
  WallCollision wallCollision;

  list<SpriteCollision> spriteCollisions;
};

struct WallSlice {
  int wallTop_px;
  int sliceH_px;
};

//===========================================
// fastTan_rp
//
// Retrieves the reciprocal of tan(a) from the lookup table
//===========================================
static double fastTan_rp(const tanMap_t& tanMap_rp, double a) {
  static const double x = static_cast<double>(tanMap_rp.size()) / (2.0 * PI);
  return tanMap_rp[static_cast<int>(normaliseAngle(a) * x)];
}

//===========================================
// fastATan
//
// Retrieves atan(x) from the lookup table
//===========================================
static double fastATan(const atanMap_t& atanMap, double x) {
  if (x < ATAN_MIN) {
    x = ATAN_MIN;
  }
  if (x > ATAN_MAX) {
    x = ATAN_MAX;
  }
  double dx = (ATAN_MAX - ATAN_MIN) / static_cast<double>(atanMap.size());
  return atanMap[static_cast<int>((x - ATAN_MIN) / dx)];
}

//===========================================
// computeF
//===========================================
static double computeF(double w, double fov) {
  return w / (2.0 * tan(0.5 * fov));
}

//===========================================
// worldPointToFloorTexel
//===========================================
static Point worldPointToFloorTexel(const Point& p, const Size& texSz_wd_rp, const Size& texSz_px) {
  double nx = p.x * texSz_wd_rp.x;
  double ny = p.y * texSz_wd_rp.y;
  return Point((nx - floor(nx)) * texSz_px.x, (ny - floor(ny)) * texSz_px.y);
}

//===========================================
// castRay
//===========================================
static void castRay(Vec2f r, const Scene& scene, double F, CastResult& result) {
  const Camera& cam = *scene.camera;
  LineSegment ray(Point(0, 0), Point(r.x * 999.9, r.y * 999.9));
  double distanceFromCamera = std::numeric_limits<double>::infinity();
  Matrix invCamMatrix = cam.matrix().inverse();

  for (auto it = scene.walls.begin(); it != scene.walls.end(); ++it) {
    const Wall& wall = **it;
    LineSegment lseg = transform(wall.lseg, invCamMatrix);

    Point pt;
    if (lineSegmentIntersect(ray, lseg, pt)) {
      // In camera space the ray will always point in positive x direction
      assert(pt.x > 0.0);

      if (pt.x < distanceFromCamera) {
        distanceFromCamera = pt.x;

        WallCollision& wallCollision = result.wallCollision;
        Collision& collision = wallCollision.collision;

        result.hitWall = true;
        wallCollision.wall = &wall;
        collision.collisionPoint = cam.matrix() * pt;
        collision.distanceAlongTarget = distance(lseg.A, pt);
        collision.distanceFromCamera = pt.x;

        LineSegment projPlane(Point(F, 0.00001 - scene.viewport.y / 2),
          Point(F, scene.viewport.y * 0.5));

        Matrix m(cam.vAngle, Vec2f(0, 0));
        LineSegment rotProjPlane = transform(projPlane, m);

        LineSegment wallRay0(Point(0, 0), Point(pt.x, -cam.height));
        LineSegment wallRay1(Point(0, 0), Point(pt.x, scene.wallHeight - cam.height));

        LineSegment projRay0(Point(0, 0), rotProjPlane.A * 999.9);
        LineSegment projRay1(Point(0, 0), rotProjPlane.B * 999.9);

        Point p;
        p = lineIntersect(wallRay0.line(), rotProjPlane.line());
        double proj_w0 = rotProjPlane.signedDistance(p.x);
        p = lineIntersect(wallRay1.line(), rotProjPlane.line());
        double proj_w1 = rotProjPlane.signedDistance(p.x);

        collision.projSliceBottom_wd = clipNumber(proj_w0, Size(0, scene.viewport.y));
        collision.projSliceTop_wd = clipNumber(proj_w1, Size(0, scene.viewport.y));

        LineSegment wall(Point(pt.x + 0.00001, -scene.wallHeight * 0.5),
          Point(pt.x, scene.wallHeight * 0.5));

        double wall_s0 = lineIntersect(projRay0.line(), wall.line()).y;
        double wall_s1 = lineIntersect(projRay1.line(), wall.line()).y;

        collision.sliceBottom_wd = clipNumber(wall.A.y, Size(wall_s0, wall_s1));
        collision.sliceTop_wd = clipNumber(wall.B.y, Size(wall_s0, wall_s1));
      }
    }
  }

  for (auto it = scene.sprites.begin(); it != scene.sprites.end(); ++it) {
    const Sprite& sprite = **it;
    Point pos = invCamMatrix * sprite.pos;
    double w = sprite.size.x;
    LineSegment lseg(Point(pos.x, pos.y - 0.5 * w), Point(pos.x * 1.00001, pos.y + 0.5 * w));

    Point pt;
    if (lineSegmentIntersect(ray, lseg, pt)) {
      SpriteCollision spriteCollision;
      Collision& collision = spriteCollision.collision;

      spriteCollision.sprite = &sprite;
      collision.collisionPoint = cam.matrix() * pt;
      collision.distanceFromCamera = pt.x;

      if (!result.hitWall || distanceFromCamera > collision.distanceFromCamera) {
        collision.distanceAlongTarget = distance(lseg.A, pt);

        LineSegment projPlane(Point(F, 0.00001 - scene.viewport.y / 2),
          Point(F, scene.viewport.y * 0.5));

        Matrix m(cam.vAngle, Vec2f(0, 0));
        LineSegment rotProjPlane = transform(projPlane, m);

        LineSegment spriteRay0(Point(0, 0), Point(pt.x, -cam.height));
        LineSegment spriteRay1(Point(0, 0), Point(pt.x, sprite.size.y - cam.height));

        LineSegment projRay0(Point(0, 0), rotProjPlane.A * 999.9);
        LineSegment projRay1(Point(0, 0), rotProjPlane.B * 999.9);

        Point p;
        p = lineIntersect(spriteRay0.line(), rotProjPlane.line());
        double proj_w0 = rotProjPlane.signedDistance(p.x);
        p = lineIntersect(spriteRay1.line(), rotProjPlane.line());
        double proj_w1 = rotProjPlane.signedDistance(p.x);

        collision.projSliceBottom_wd = clipNumber(proj_w0, Size(0, scene.viewport.y));
        collision.projSliceTop_wd = clipNumber(proj_w1, Size(0, scene.viewport.y));

        LineSegment spriteLseg(Point(pt.x + 0.00001, -cam.height),
          Point(pt.x, sprite.size.y - cam.height));

        double sprite_s0 = lineIntersect(projRay0.line(), spriteLseg.line()).y;
        double sprite_s1 = lineIntersect(projRay1.line(), spriteLseg.line()).y;

        collision.sliceBottom_wd = clipNumber(spriteLseg.A.y, Size(sprite_s0, sprite_s1));
        collision.sliceTop_wd = clipNumber(spriteLseg.B.y, Size(sprite_s0, sprite_s1));

        result.spriteCollisions.push_back(spriteCollision);
      }
    }
  }

  result.spriteCollisions.sort([](const SpriteCollision& a, const SpriteCollision& b) {
    return a.collision.distanceFromCamera > b.collision.distanceFromCamera;
  });
}

//===========================================
// drawCeilingSlice
//===========================================
static void drawCeilingSlice(QImage& target, const Scene& scene, const Point& collisionPoint,
  int wallTop_px, int screenX_px, int screenH_px, double projX_wd, double vWorldUnitsInPx,
  double F, const tanMap_t& tanMap_rp, const atanMap_t& atanMap) {

  const Camera& cam = *scene.camera;
  const QImage& ceilingTex = scene.textures.at("ceiling");

  double hAngle = atan(projX_wd / F);
  LineSegment ray(cam.pos, collisionPoint);

  Size texSz_px(ceilingTex.rect().width(), ceilingTex.rect().height());
  double texelInWorldUnits = scene.wallHeight / texSz_px.y;
  Size texSz_wd_rp(1.0 / (texSz_px.x * texelInWorldUnits), 1.0 / (texSz_px.y * texelInWorldUnits));

  double vWorldUnitsInPx_rp = 1.0 / vWorldUnitsInPx;
  double F_rp = 1.0 / F;
  double rayLen_rp = 1.0 / ray.length();
  double cosHAngle_rp = 1.0 / cos(hAngle);

  for (int j = wallTop_px; j >= 0; --j) {
    double projY_wd = (screenH_px * 0.5 - j) * vWorldUnitsInPx_rp;
    double vAngle = fastATan(atanMap, projY_wd * F_rp) + cam.vAngle;
    double d_ = 0.5 * scene.wallHeight * fastTan_rp(tanMap_rp, vAngle);
    double d = d_ * cosHAngle_rp;
    double s = d * rayLen_rp;
    Point p(ray.A.x + (ray.B.x - ray.A.x) * s, ray.A.y + (ray.B.y - ray.A.y) * s);

    Point texel = worldPointToFloorTexel(p, texSz_wd_rp, texSz_px);

    QRgb* pixels = reinterpret_cast<QRgb*>(target.scanLine(j));
    pixels[screenX_px] = ceilingTex.pixel(texel.x, texel.y);
  }
}

//===========================================
// drawFloorSlice
//===========================================
static void drawFloorSlice(QImage& target, const Scene& scene, const Point& collisionPoint,
  int wallBottom_px, int screenX_px, int screenH_px, double projX_wd, double vWorldUnitsInPx,
  double F, const tanMap_t& tanMap_rp, const atanMap_t& atanMap) {

  const Camera& cam = *scene.camera;
  const QImage& floorTex = scene.textures.at("floor");

  double hAngle = atan(projX_wd / F);
  LineSegment ray(cam.pos, collisionPoint);

  Size texSz_px(floorTex.rect().width(), floorTex.rect().height());
  double texelInWorldUnits = scene.wallHeight / texSz_px.y;
  Size texSz_wd_rp(1.0 / (texSz_px.x * texelInWorldUnits), 1.0 / (texSz_px.y * texelInWorldUnits));

  double vWorldUnitsInPx_rp = 1.0 / vWorldUnitsInPx;
  double F_rp = 1.0 / F;
  double rayLen_rp = 1.0 / ray.length();
  double cosHAngle_rp = 1.0 / cos(hAngle);

  for (int j = wallBottom_px; j < screenH_px; ++j) {
    double projY_wd = (j - screenH_px * 0.5) * vWorldUnitsInPx_rp;
    double vAngle = fastATan(atanMap, projY_wd * F_rp) - cam.vAngle;
    double d_ = 0.5 * scene.wallHeight * fastTan_rp(tanMap_rp, vAngle);
    double d = d_ * cosHAngle_rp;
    double s = d * rayLen_rp;
    Point p(ray.A.x + (ray.B.x - ray.A.x) * s, ray.A.y + (ray.B.y - ray.A.y) * s);

    Point texel = worldPointToFloorTexel(p, texSz_wd_rp, texSz_px);

    QRgb* pixels = reinterpret_cast<QRgb*>(target.scanLine(j));
    pixels[screenX_px] = floorTex.pixel(texel.x, texel.y);
  }
}

//===========================================
// sampleTexture
//===========================================
static QRect sampleTexture(const QRect& rect, const Collision& collision, double camHeight,
  double width_wd, double height_wd) {

  double H_px = rect.height();
  double W_px = rect.width();

  double hWorldUnit_px = W_px / width_wd;
  double texW_wd = W_px / hWorldUnit_px;

  double n = collision.distanceAlongTarget / texW_wd;
  double x = (n - floor(n)) * texW_wd;

  double texBottom_px = H_px - ((camHeight + collision.sliceBottom_wd) / height_wd) * H_px;
  double texTop_px = H_px - ((camHeight + collision.sliceTop_wd) / height_wd) * H_px;

  int i = x * hWorldUnit_px;

  return QRect(rect.x() + i, rect.y() + texTop_px, 1, texBottom_px - texTop_px);
}

//===========================================
// drawWallSlice
//===========================================
static WallSlice drawWallSlice(QPainter& painter, const Scene& scene,
  const WallCollision& collision, double F, int screenX_px, int screenH_px,
  double vWorldUnitsInPx) {

  const QImage& wallTex = scene.textures.at(collision.wall->texture);

  int screenSliceBottom_px = screenH_px - collision.collision.projSliceBottom_wd * vWorldUnitsInPx;
  int screenSliceTop_px = screenH_px - collision.collision.projSliceTop_wd * vWorldUnitsInPx;

  QRect trgRect(screenX_px, screenSliceTop_px, 1, screenSliceBottom_px - screenSliceTop_px);
  QRect srcRect = sampleTexture(wallTex.rect(), collision.collision, scene.camera->height,
    scene.wallHeight, scene.wallHeight);

  painter.drawImage(trgRect, wallTex, srcRect);

  return WallSlice{screenSliceTop_px, screenSliceBottom_px - screenSliceTop_px};
}

//===========================================
// drawSprites
//===========================================
static void drawSprites(QPainter& painter, const Scene& scene, const CastResult& result,
  const Camera& cam, double F, double screenX_px, double screenH_px, double vWorldUnitsInPx) {

  for (auto it = result.spriteCollisions.begin(); it != result.spriteCollisions.end(); ++it) {
    const SpriteCollision& collision = *it;
    const Sprite& sprite = *collision.sprite;

    const QImage& tex = scene.textures.at(sprite.texture);
    const QRectF& uv = sprite.textureRegion(cam.pos);
    QRect r = tex.rect();
    QRect frame(r.width() * uv.x(), r.height() * uv.y(), r.width() * uv.width(),
      r.height() * uv.height());

    int screenSliceBottom_px = screenH_px - collision.collision.projSliceBottom_wd
      * vWorldUnitsInPx;
    int screenSliceTop_px = screenH_px - collision.collision.projSliceTop_wd * vWorldUnitsInPx;

    QRect srcRect = sampleTexture(frame, collision.collision, scene.camera->height, sprite.size.x,
      sprite.size.y);
    QRect trgRect(screenX_px, screenSliceTop_px, 1, screenSliceBottom_px - screenSliceTop_px);

    painter.drawImage(trgRect, tex, srcRect);
  }
}

//===========================================
// Renderer::Renderer
//===========================================
Renderer::Renderer() {
  for (unsigned int i = 0; i < m_tanMap_rp.size(); ++i) {
    m_tanMap_rp[i] = 1.0 / tan(2.0 * PI * static_cast<double>(i)
      / static_cast<double>(m_tanMap_rp.size()));
  }

  double dx = (ATAN_MAX - ATAN_MIN) / static_cast<double>(m_atanMap.size());
  for (unsigned int i = 0; i < m_atanMap.size(); ++i) {
    m_atanMap[i] = atan(ATAN_MIN + dx * static_cast<double>(i));
  }
}

//===========================================
// Renderer::renderScene
//===========================================
void Renderer::renderScene(QImage& target, const Scene& scene) {
  QPainter painter;
  painter.begin(&target);

  const Camera& cam = *scene.camera;

  int screenW_px = target.width();
  int screenH_px = target.height();

  double hWorldUnitsInPx = screenW_px / scene.viewport.x;
  double vWorldUnitsInPx = screenH_px / scene.viewport.y;

  QRect rect(QPoint(), QSize(screenW_px, screenH_px));
  painter.fillRect(rect, QBrush(QColor(0, 0, 0)));

  double F = computeF(scene.viewport.x, cam.hFov);

  for (int screenX_px = 0; screenX_px < screenW_px; ++screenX_px) {
    double projX_wd = static_cast<double>(screenX_px - screenW_px / 2) / hWorldUnitsInPx;

    CastResult result;
    castRay(Vec2f(F, projX_wd), scene, F, result);

    if (result.hitWall) {
      WallCollision& collision = result.wallCollision;

      WallSlice slice = drawWallSlice(painter, scene, collision, F, screenX_px, screenH_px,
        vWorldUnitsInPx);

      drawFloorSlice(target, scene, collision.collision.collisionPoint,
        slice.wallTop_px + slice.sliceH_px, screenX_px, screenH_px, projX_wd, vWorldUnitsInPx,
        F, m_tanMap_rp, m_atanMap);
      drawCeilingSlice(target, scene, collision.collision.collisionPoint, slice.wallTop_px,
        screenX_px, screenH_px, projX_wd, vWorldUnitsInPx, F, m_tanMap_rp, m_atanMap);
    }

    drawSprites(painter, scene, result, cam, F, screenX_px, screenH_px, vWorldUnitsInPx);
  }

  painter.end();
}
