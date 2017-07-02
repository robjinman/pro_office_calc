#include <cmath>
#include <cassert>
#include <limits>
#include <list>
#include <vector>
#include <QPainter>
#include <QPaintDevice>
#include <QBrush>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"
#include "exception.hpp"


using std::string;
using std::list;
using std::vector;
using std::array;
using std::unique_ptr;


static const double ATAN_MIN = -10.0;
static const double ATAN_MAX = 10.0;


struct ScreenSlice {
  int sliceBottom_px;
  int sliceTop_px;
  int viewportBottom_px;
  int viewportTop_px;
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
// worldPointToFloorTexel
//===========================================
static Point worldPointToFloorTexel(const Point& p, const Size& texSz_wd_rp, const Size& texSz_px) {
  double nx = p.x * texSz_wd_rp.x;
  double ny = p.y * texSz_wd_rp.y;

  if (nx < 0 || std::isinf(nx) || std::isnan(nx)) {
    nx = 0;
  }
  if (ny < 0 || std::isinf(ny) || std::isnan(ny)) {
    ny = 0;
  }

  return Point((nx - floor(nx)) * texSz_px.x, (ny - floor(ny)) * texSz_px.y);
}

//===========================================
// constructIntersection
//===========================================
static Intersection* constructIntersection(EdgeKind kind) {
  switch (kind) {
    case EdgeKind::WALL:
      return new WallX;
      break;
    case EdgeKind::JOINING_EDGE:
      return new JoiningEdgeX;
      break;
    default:
      EXCEPTION("Error constructing Intersection of unknown type");
  }
}

//===========================================
// findIntersections_r
//===========================================
void findIntersections_r(const Camera& camera, const LineSegment& ray, const Region& region,
  const Edge* exclude, CastResult& result) {

  for (auto it = region.children.begin(); it != region.children.end(); ++it) {
    findIntersections_r(camera, ray, **it, nullptr, result);
  }

  Matrix invCamMatrix = camera.matrix().inverse();

  for (auto it = region.sprites.begin(); it != region.sprites.end(); ++it) {
    Sprite& sprite = **it;
    Point pos = invCamMatrix * sprite.pos;
    double w = sprite.size.x;
    LineSegment lseg(Point(pos.x, pos.y - 0.5 * w), Point(pos.x * 1.00001, pos.y + 0.5 * w));

    Point pt;
    if (lineSegmentIntersect(ray, lseg, pt)) {
      SpriteX* X = new SpriteX;
      result.intersections.push_back(pIntersection_t(X));

      X->point_cam = pt;
      X->point_world = camera.matrix() * pt;
      X->distanceFromCamera = pt.x;
      X->distanceAlongTarget = distance(lseg.A, pt);
      X->sprite = &sprite;
    }
  }

  for (auto it = region.edges.begin(); it != region.edges.end(); ++it) {
    Edge& edge = **it;

    LineSegment lseg = transform(edge.lseg, invCamMatrix);

    Point pt;
    if (lineSegmentIntersect(ray, lseg, pt)) {
      if (exclude != &edge) {
        Intersection* X = constructIntersection(edge.kind);
        result.intersections.push_back(pIntersection_t(X));

        X->point_cam = pt;
        X->point_world = camera.matrix() * pt;
        X->distanceFromCamera = pt.x;
        X->distanceAlongTarget = distance(lseg.A, pt);

        if (edge.kind == EdgeKind::WALL) {
          WallX* wallX = dynamic_cast<WallX*>(X);
          Wall& wall = dynamic_cast<Wall&>(edge);

          wallX->wall = &wall;
        }
        else if (edge.kind == EdgeKind::JOINING_EDGE) {
          JoiningEdgeX* jeX = dynamic_cast<JoiningEdgeX*>(X);
          JoiningEdge& je = dynamic_cast<JoiningEdge&>(edge);

          jeX->joiningEdge = &je;

          const Region& next = je.regionA == &region ? *je.regionB : *je.regionA;
          findIntersections_r(camera, ray, next, &edge, result);
        }
      }
    }
  }
}

//===========================================
// castRay
//===========================================
static void castRay(Vec2f r, const Scene& scene, CastResult& result) {
  auto& intersections = result.intersections;

  const Camera& cam = *scene.camera;
  LineSegment ray(Point(0, 0), Point(r.x * 999.9, r.y * 999.9));

  findIntersections_r(cam, ray, *scene.currentRegion, nullptr, result);

  intersections.sort([](const pIntersection_t& a, const pIntersection_t& b) {
    return a->distanceFromCamera < b->distanceFromCamera;
  });

  LineSegment projPlane(Point(cam.F, 0.00001 - scene.viewport.y / 2),
    Point(cam.F, scene.viewport.y * 0.5));

  Matrix m(cam.vAngle, Vec2f(0, 0));
  LineSegment rotProjPlane = transform(projPlane, m);

  LineSegment projRay0(Point(0, 0), rotProjPlane.A * 999.9);
  LineSegment projRay1(Point(0, 0), rotProjPlane.B * 999.9);
  double subview0 = 0;
  double subview1 = scene.viewport.y;

  const Region* region = scene.currentRegion;
  int last = -1;
  for (auto it = intersections.begin(); it != intersections.end(); ++it) {
    ++last;

    if ((*it)->kind == IntersectionKind::WALL) {
      WallX& X = dynamic_cast<WallX&>(**it);

      double floorHeight = X.wall->region->floorHeight;
      double targetHeight = X.wall->region->ceilingHeight - X.wall->region->floorHeight;
      const Point& pt = X.point_cam;

      LineSegment wall(Point(pt.x + 0.00001, floorHeight - cam.height),
        Point(pt.x, floorHeight - cam.height + targetHeight));

      LineSegment wallRay0(Point(0, 0), Point(pt.x, wall.A.y));
      LineSegment wallRay1(Point(0, 0), Point(pt.x, wall.B.y));

      Point p = lineIntersect(wallRay0.line(), rotProjPlane.line());
      double proj_w0 = rotProjPlane.signedDistance(p.x);
      p = lineIntersect(wallRay1.line(), rotProjPlane.line());
      double proj_w1 = rotProjPlane.signedDistance(p.x);

      if (!isBetween(proj_w0, subview0, subview1)) {
        proj_w0 = subview0;
      }
      if (!isBetween(proj_w1, subview0, subview1)) {
        proj_w1 = subview1;
      }

      X.slice.projSliceBottom_wd = proj_w0;
      X.slice.projSliceTop_wd = proj_w1;

      X.slice.viewportBottom_wd = subview0;
      X.slice.viewportTop_wd = subview1;

      double wall_s0 = lineIntersect(projRay0.line(), wall.line()).y;
      double wall_s1 = lineIntersect(projRay1.line(), wall.line()).y;

      X.slice.sliceBottom_wd = clipNumber(wall.A.y, Size(wall_s0, wall_s1));
      X.slice.sliceTop_wd = clipNumber(wall.B.y, Size(wall_s0, wall_s1));

      break;
    }
    else if ((*it)->kind == IntersectionKind::JOINING_EDGE) {
      JoiningEdgeX& X = dynamic_cast<JoiningEdgeX&>(**it);

      assert(region == X.joiningEdge->regionA || region == X.joiningEdge->regionB);
      Region* nextRegion = region == X.joiningEdge->regionA ?
        X.joiningEdge->regionB : X.joiningEdge->regionA;

      X.nearRegion = region;
      X.farRegion = nextRegion;

      const Point& pt = X.point_cam;

      double floorDiff = nextRegion->floorHeight - region->floorHeight;
      double ceilingDiff = region->ceilingHeight - nextRegion->ceilingHeight;
      double nextRegionSpan = nextRegion->ceilingHeight - nextRegion->floorHeight;

      double bottomWallA = region->floorHeight - cam.height;
      double bottomWallB = bottomWallA + floorDiff;
      double topWallA = bottomWallB + nextRegionSpan;
      double topWallB = topWallA + ceilingDiff;

      if (floorDiff < 0) {
        bottomWallB = bottomWallA;
      }
      if (ceilingDiff < 0) {
        topWallA = topWallB;
      }

      LineSegment bottomWall(Point(pt.x + 0.00001, bottomWallA), Point(pt.x, bottomWallB));
      LineSegment topWall(Point(pt.x + 0.00001, topWallA), Point(pt.x, topWallB));

      LineSegment bottomWallRay0(Point(0, 0), bottomWall.A);
      LineSegment bottomWallRay1(Point(0, 0), bottomWall.B);

      LineSegment topWallRay0(Point(0, 0), topWall.A);
      LineSegment topWallRay1(Point(0, 0), topWall.B);

      Point p = lineIntersect(bottomWallRay0.line(), rotProjPlane.line());
      double proj_bw0 = rotProjPlane.signedDistance(p.x);
      p = lineIntersect(bottomWallRay1.line(), rotProjPlane.line());
      Point vw0 = p = clipToLineSegment(p, rotProjPlane);
      double proj_bw1 = rotProjPlane.signedDistance(p.x);
      p = lineIntersect(topWallRay0.line(), rotProjPlane.line());
      Point vw1 = p = clipToLineSegment(p, rotProjPlane);
      double proj_tw0 = rotProjPlane.signedDistance(p.x);
      p = lineIntersect(topWallRay1.line(), rotProjPlane.line());
      double proj_tw1 = rotProjPlane.signedDistance(p.x);

/*
if (!isBetween(proj_bw0, subview0, subview1)) {
  proj_bw0 = subview0;
}
if (!isBetween(proj_bw1, subview0, subview1)) {
  proj_bw1 = subview0;
}
if (!isBetween(proj_tw0, subview0, subview1)) {
  proj_tw0 = subview1;
}
if (!isBetween(proj_tw1, subview0, subview1)) {
  proj_tw1 = subview1;
}

X.slice0.projSliceBottom_wd = proj_bw0;
X.slice0.projSliceTop_wd = proj_bw1;
X.slice1.projSliceBottom_wd = proj_tw0;
X.slice1.projSliceTop_wd = proj_tw1;
*/

      X.slice0.projSliceBottom_wd = clipNumber(proj_bw0, Size(subview0, subview1));
      X.slice0.projSliceTop_wd = clipNumber(proj_bw1, Size(subview0, subview1));

      X.slice1.projSliceBottom_wd = clipNumber(proj_tw0, Size(subview0, subview1));
      X.slice1.projSliceTop_wd = clipNumber(proj_tw1, Size(subview0, subview1));

      LineSegment wall(Point(pt.x + 0.00001, bottomWallA), Point(pt.x, topWallB));
      double wall_s0 = lineIntersect(projRay0.line(), wall.line()).y;
      double wall_s1 = lineIntersect(projRay1.line(), wall.line()).y;

      X.slice0.sliceBottom_wd = clipNumber(bottomWall.A.y, Size(wall_s0, wall_s1));
      X.slice0.sliceTop_wd = clipNumber(bottomWall.B.y, Size(wall_s0, wall_s1));

      X.slice1.sliceBottom_wd = clipNumber(topWall.A.y, Size(wall_s0, wall_s1));
      X.slice1.sliceTop_wd = clipNumber(topWall.B.y, Size(wall_s0, wall_s1));

      X.slice0.viewportBottom_wd = subview0;
      X.slice0.viewportTop_wd = subview1;
      X.slice1.viewportBottom_wd = subview0;
      X.slice1.viewportTop_wd = subview1;

      subview0 = proj_bw1;
      subview1 = proj_tw0;

      projRay0 = LineSegment(Point(0, 0), vw0 * 999.9);
      projRay1 = LineSegment(Point(0, 0), vw1 * 999.9);

      region = nextRegion;
    }
    else if ((*it)->kind == IntersectionKind::SPRITE) {
      SpriteX& X = dynamic_cast<SpriteX&>(**it);

      double floorHeight = region->floorHeight;
      const Point& pt = X.point_cam;

      LineSegment wall(Point(pt.x + 0.00001, floorHeight - cam.height),
        Point(pt.x, floorHeight - cam.height + X.sprite->size.y));

      LineSegment wallRay0(Point(0, 0), Point(pt.x, wall.A.y));
      LineSegment wallRay1(Point(0, 0), Point(pt.x, wall.B.y));

      Point p = lineIntersect(wallRay0.line(), rotProjPlane.line());
      double proj_w0 = rotProjPlane.signedDistance(p.x);
      p = lineIntersect(wallRay1.line(), rotProjPlane.line());
      double proj_w1 = rotProjPlane.signedDistance(p.x);

      X.slice.viewportBottom_wd = subview0;
      X.slice.viewportTop_wd = subview1;

      X.slice.projSliceBottom_wd = clipNumber(proj_w0, Size(subview0, subview1));
      X.slice.projSliceTop_wd = clipNumber(proj_w1, Size(subview0, subview1));

      double wall_s0 = lineIntersect(projRay0.line(), wall.line()).y;
      double wall_s1 = lineIntersect(projRay1.line(), wall.line()).y;

      X.slice.sliceBottom_wd = clipNumber(wall.A.y, Size(wall_s0, wall_s1));
      X.slice.sliceTop_wd = clipNumber(wall.B.y, Size(wall_s0, wall_s1));
    }

    if (subview1 <= subview0) {
      break;
    }
  }

  if (intersections.size() > 0) {
    intersections.resize(last + 1);
  }
}

//===========================================
// drawCeilingSlice
//===========================================
static void drawCeilingSlice(QImage& target, const Scene& scene, double ceilingHeight,
  const Point& collisionPoint, const ScreenSlice& slice, int screenX_px, double projX_wd,
  double vWorldUnit_px, const tanMap_t& tanMap_rp, const atanMap_t& atanMap) {

  double screenH_px = scene.viewport.y * vWorldUnit_px;
  const Camera& cam = *scene.camera;
  const QImage& ceilingTex = scene.textures.at("ceiling");

  double hAngle = atan(projX_wd / cam.F);
  LineSegment ray(cam.pos, collisionPoint);

  Size texSz_px(ceilingTex.rect().width(), ceilingTex.rect().height());
  double texelInWorldUnits = scene.wallHeight / texSz_px.y;
  Size texSz_wd_rp(1.0 / (texSz_px.x * texelInWorldUnits), 1.0 / (texSz_px.y * texelInWorldUnits));

  double vWorldUnit_px_rp = 1.0 / vWorldUnit_px;
  double F_rp = 1.0 / cam.F;
  double rayLen_rp = 1.0 / ray.length();
  double cosHAngle_rp = 1.0 / cos(hAngle);

  for (int j = slice.sliceTop_px; j >= slice.viewportTop_px; --j) {
    double projY_wd = (screenH_px * 0.5 - j) * vWorldUnit_px_rp;
    double vAngle = fastATan(atanMap, projY_wd * F_rp) + cam.vAngle;
    double d_ = (ceilingHeight - cam.height) * fastTan_rp(tanMap_rp, vAngle);
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
static void drawFloorSlice(QImage& target, const Scene& scene, double floorHeight,
  const Point& collisionPoint, const ScreenSlice& slice, int screenX_px, double projX_wd,
  double vWorldUnit_px, const tanMap_t& tanMap_rp, const atanMap_t& atanMap) {

  double screenH_px = scene.viewport.y * vWorldUnit_px;
  const Camera& cam = *scene.camera;
  const QImage& floorTex = scene.textures.at("floor");

  double hAngle = atan(projX_wd / scene.camera->F);
  LineSegment ray(cam.pos, collisionPoint);

  Size texSz_px(floorTex.rect().width(), floorTex.rect().height());
  double texelInWorldUnits = scene.wallHeight / texSz_px.y;
  Size texSz_wd_rp(1.0 / (texSz_px.x * texelInWorldUnits), 1.0 / (texSz_px.y * texelInWorldUnits));

  double vWorldUnit_px_rp = 1.0 / vWorldUnit_px;
  double F_rp = 1.0 / scene.camera->F;
  double rayLen_rp = 1.0 / ray.length();
  double cosHAngle_rp = 1.0 / cos(hAngle);

  for (int j = slice.sliceBottom_px; j < slice.viewportBottom_px; ++j) {
    double projY_wd = (j - screenH_px * 0.5) * vWorldUnit_px_rp;
    double vAngle = fastATan(atanMap, projY_wd * F_rp) - cam.vAngle;
    double d_ = (cam.height - floorHeight) * fastTan_rp(tanMap_rp, vAngle);
    double d = d_ * cosHAngle_rp;
    double s = d * rayLen_rp;
    Point p(ray.A.x + (ray.B.x - ray.A.x) * s, ray.A.y + (ray.B.y - ray.A.y) * s);

    Point texel = worldPointToFloorTexel(p, texSz_wd_rp, texSz_px);

    QRgb* pixels = reinterpret_cast<QRgb*>(target.scanLine(j));
    pixels[screenX_px] = floorTex.pixel(texel.x, texel.y);
  }
}

//===========================================
// sampleWallTexture
//===========================================
static void sampleWallTexture(const QRect& texRect, double camHeight_wd, const Size& viewport_px,
  double screenX_px, double hWorldUnit_px, double vWorldUnit_px, double distanceAlongTarget,
  const Slice& slice, double width_wd, double height_wd, vector<QRectF>& trgRects,
  vector<QRect>& srcRects) {

  double H_tx = texRect.height();
  double W_tx = texRect.width();

  double hWorldUnit_tx = W_tx / width_wd;
  double vWorldUnit_tx = H_tx / height_wd;

  double projSliceH_wd = slice.projSliceTop_wd - slice.projSliceBottom_wd;
  double sliceH_wd = slice.sliceTop_wd - slice.sliceBottom_wd;
  double sliceToProjScale = projSliceH_wd / sliceH_wd;
  double projTexH_wd = height_wd * sliceToProjScale;

  // World space
  double sliceBottom_wd = slice.sliceBottom_wd + camHeight_wd;
  double sliceTop_wd = slice.sliceTop_wd + camHeight_wd;

  // World space (not camera space)
  auto fnSliceToProjY = [&](double y) {
    return slice.projSliceBottom_wd + (y - sliceBottom_wd) * sliceToProjScale;
  };

  auto fnProjToScreenY = [&](double y) {
    return viewport_px.y - (y * vWorldUnit_px);
  };

  double nx = distanceAlongTarget / width_wd;
  double x = (nx - floor(nx)) * width_wd;

  int j0 = floor(sliceBottom_wd / height_wd);
  int j1 = ceil(sliceTop_wd / height_wd);

  double bottomOffset_wd = sliceBottom_wd - j0 * height_wd;
  double topOffset_wd = j1 * height_wd - sliceTop_wd;

  for (int j = j0; j < j1; ++j) {
    QRect srcRect;
    srcRect.setX(texRect.x() + x * hWorldUnit_tx);
    srcRect.setY(texRect.y());
    srcRect.setWidth(1);
    srcRect.setHeight(texRect.height());

    double y = j * height_wd;

    QRectF trgRect;
    trgRect.setX(screenX_px);
    trgRect.setY(fnProjToScreenY(fnSliceToProjY(y) + projTexH_wd));
    trgRect.setWidth(1);
    trgRect.setHeight(projTexH_wd * vWorldUnit_px);

    // Note that screen y-axis is inverted
    if (j == j0) {
      srcRect.setHeight(srcRect.height() - bottomOffset_wd * vWorldUnit_tx);
      trgRect.setHeight(trgRect.height() - bottomOffset_wd * sliceToProjScale * vWorldUnit_px);
    }
    if (j + 1 == j1) {
      // QRect::setY() also changes the height!
      srcRect.setY(srcRect.y() + topOffset_wd * vWorldUnit_tx);
      trgRect.setY(trgRect.y() + topOffset_wd * sliceToProjScale * vWorldUnit_px);
    }

    srcRects.push_back(srcRect);
    trgRects.push_back(trgRect);
  }
}

//===========================================
// sampleSpriteTexture
//===========================================
static QRect sampleSpriteTexture(const QRect& rect, const SpriteX& X, double camHeight,
  double width_wd, double height_wd, double y_wd) {

  double H_px = rect.height();
  double W_px = rect.width();

  double hWorldUnit_px = W_px / width_wd;
  double texW_wd = W_px / hWorldUnit_px;

  double n = X.distanceAlongTarget / texW_wd;
  double x = (n - floor(n)) * texW_wd;

  double texBottom_px = H_px - ((camHeight + X.slice.sliceBottom_wd - y_wd) / height_wd) * H_px;
  double texTop_px = H_px - ((camHeight + X.slice.sliceTop_wd - y_wd) / height_wd) * H_px;

  int i = x * hWorldUnit_px;

  return QRect(rect.x() + i, rect.y() + texTop_px, 1, texBottom_px - texTop_px);
}

//===========================================
// drawSlice
//===========================================
static ScreenSlice drawSlice(QPainter& painter, const Scene& scene, double F,
  double distanceAlongTarget, const Slice& slice, const string& texture, double screenX_px,
  const Size& viewport_px) {

  double hWorldUnit_px = viewport_px.x / scene.viewport.x;
  double vWorldUnit_px = viewport_px.y / scene.viewport.y;

  const QImage& wallTex = scene.textures.at(texture);

  int screenSliceBottom_px = viewport_px.y - slice.projSliceBottom_wd * vWorldUnit_px;
  int screenSliceTop_px = viewport_px.y - slice.projSliceTop_wd * vWorldUnit_px;

  screenSliceBottom_px = clipNumber(screenSliceBottom_px, Size(0, viewport_px.y - 1));
  screenSliceTop_px = clipNumber(screenSliceTop_px, Size(0, viewport_px.y - 1));

  vector<QRect> srcRects;
  vector<QRectF> trgRects;
  sampleWallTexture(wallTex.rect(), scene.camera->height, viewport_px, screenX_px, hWorldUnit_px,
    vWorldUnit_px, distanceAlongTarget, slice, scene.wallHeight, scene.wallHeight, trgRects,
    srcRects);

  assert(srcRects.size() == trgRects.size());

  for (unsigned int i = 0; i < srcRects.size(); ++i) {
    painter.drawImage(trgRects[i], wallTex, srcRects[i]);
  }

  int viewportBottom_px = (scene.viewport.y - slice.viewportBottom_wd) * vWorldUnit_px;
  int viewportTop_px = (scene.viewport.y - slice.viewportTop_wd) * vWorldUnit_px;

  viewportBottom_px = clipNumber(viewportBottom_px, Size(0, viewport_px.y - 1));
  viewportTop_px = clipNumber(viewportTop_px, Size(0, viewport_px.y - 1));

  return ScreenSlice{screenSliceBottom_px, screenSliceTop_px, viewportBottom_px, viewportTop_px};
}

//===========================================
// drawSprite
//===========================================
static void drawSprite(QPainter& painter, const Scene& scene, const Size& viewport_px,
  const SpriteX& spriteX, double screenX_px) {

  double vWorldUnit_px = viewport_px.y / scene.viewport.y;

  const Sprite& sprite = *spriteX.sprite;
  const Slice& slice = spriteX.slice;

  const QImage& tex = scene.textures.at(sprite.texture);
  const QRectF& uv = sprite.textureRegion(scene.camera->pos);
  QRect r = tex.rect();
  QRect frame(r.width() * uv.x(), r.height() * uv.y(), r.width() * uv.width(),
    r.height() * uv.height());

  int screenSliceBottom_px = viewport_px.y - slice.projSliceBottom_wd * vWorldUnit_px;
  int screenSliceTop_px = viewport_px.y - slice.projSliceTop_wd * vWorldUnit_px;

  QRect srcRect = sampleSpriteTexture(frame, spriteX, scene.camera->height, sprite.size.x,
    sprite.size.y, sprite.region->floorHeight);
  QRect trgRect(screenX_px, screenSliceTop_px, 1, screenSliceBottom_px - screenSliceTop_px);

  painter.drawImage(trgRect, tex, srcRect);
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

  Size viewport_px(target.width(), target.height());
  const Camera& cam = *scene.camera;

  double hWorldUnit_px = viewport_px.x / scene.viewport.x;
  double vWorldUnit_px = viewport_px.y / scene.viewport.y;

  QRect rect(QPoint(), QSize(viewport_px.x, viewport_px.y));
  painter.fillRect(rect, QBrush(QColor(0, 0, 0)));

  for (int screenX_px = 0; screenX_px < viewport_px.x; ++screenX_px) {
    double projX_wd = static_cast<double>(screenX_px - viewport_px.x / 2) / hWorldUnit_px;

    CastResult result;
    castRay(Vec2f(cam.F, projX_wd), scene, result);

    for (auto it = result.intersections.rbegin(); it != result.intersections.rend(); ++it) {
      Intersection& X = **it;

      if (X.kind == IntersectionKind::WALL) {
        const WallX& wallX = dynamic_cast<const WallX&>(X);

        ScreenSlice slice = drawSlice(painter, scene, cam.F, wallX.distanceAlongTarget, wallX.slice,
          wallX.wall->texture, screenX_px, viewport_px);

        drawFloorSlice(target, scene, wallX.wall->region->floorHeight, wallX.point_world, slice,
          screenX_px, projX_wd, vWorldUnit_px, m_tanMap_rp, m_atanMap);
        drawCeilingSlice(target, scene, wallX.wall->region->ceilingHeight, wallX.point_world, slice,
          screenX_px, projX_wd, vWorldUnit_px, m_tanMap_rp, m_atanMap);
      }
      else if (X.kind == IntersectionKind::JOINING_EDGE) {
        const JoiningEdgeX& jeX = dynamic_cast<const JoiningEdgeX&>(X);

        ScreenSlice slice0 = drawSlice(painter, scene, cam.F, jeX.distanceAlongTarget, jeX.slice0,
          jeX.joiningEdge->bottomTexture, screenX_px, viewport_px);

        drawFloorSlice(target, scene, jeX.nearRegion->floorHeight, jeX.point_world, slice0,
          screenX_px, projX_wd, vWorldUnit_px, m_tanMap_rp, m_atanMap);

        ScreenSlice slice1 = drawSlice(painter, scene, cam.F, jeX.distanceAlongTarget, jeX.slice1,
          jeX.joiningEdge->topTexture, screenX_px, viewport_px);

        drawCeilingSlice(target, scene, jeX.nearRegion->ceilingHeight, jeX.point_world, slice1,
          screenX_px, projX_wd, vWorldUnit_px, m_tanMap_rp, m_atanMap);
      }
      else if (X.kind == IntersectionKind::SPRITE) {
        const SpriteX& spriteX = dynamic_cast<const SpriteX&>(X);
        drawSprite(painter, scene, viewport_px, spriteX, screenX_px);
      }
    }
  }

  painter.end();
}
