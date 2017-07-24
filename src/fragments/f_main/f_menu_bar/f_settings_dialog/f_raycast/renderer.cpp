#include <cmath>
#include <cassert>
#include <limits>
#include <list>
#include <set>
#include <vector>
#include <ostream>
#include <functional>
#include <QPainter>
#include <QPaintDevice>
#include <QBrush>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_graph.hpp"
#include "exception.hpp"


using std::string;
using std::list;
using std::set;
using std::vector;
using std::array;
using std::unique_ptr;
using std::ostream;
using std::function;


static const double ATAN_MIN = -10.0;
static const double ATAN_MAX = 10.0;


struct ScreenSlice {
  int sliceBottom_px;
  int sliceTop_px;
  int viewportBottom_px;
  int viewportTop_px;
};

ostream& operator<<(ostream& os, CRenderKind kind) {
  switch (kind) {
    case CRenderKind::REGION: os << "REGION"; break;
    case CRenderKind::WALL: os << "WALL"; break;
    case CRenderKind::JOINING_EDGE: os << "JOINING_EDGE"; break;
    case CRenderKind::FLOOR_DECAL: os << "FLOOR_DECAL"; break;
    case CRenderKind::WALL_DECAL: os << "WALL_DECAL"; break;
    case CRenderKind::SPRITE: os << "SPRITE"; break;
  }
  return os;
}

//===========================================
// forEachConstCRegion
//===========================================
void forEachConstCRegion(const CRegion& region, function<void(const CRegion&)> fn) {
  fn(region);
  std::for_each(region.children.begin(), region.children.end(),
    [&](const unique_ptr<CRegion>& r) {

    forEachConstCRegion(*r, fn);
  });
}

//===========================================
// forEachCRegion
//===========================================
void forEachCRegion(CRegion& region, function<void(CRegion&)> fn) {
  fn(region);
  std::for_each(region.children.begin(), region.children.end(),
    [&](unique_ptr<CRegion>& r) {

    forEachCRegion(*r, fn);
  });
}

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
static Intersection* constructIntersection(CRenderKind kind) {
  switch (kind) {
    case CRenderKind::WALL:
      return new WallX;
      break;
    case CRenderKind::JOINING_EDGE:
      return new JoiningEdgeX;
      break;
    default:
      EXCEPTION("Error constructing Intersection of unknown type");
  }
}

//===========================================
// findIntersections_r
//===========================================
void findIntersections_r(const Camera& camera, const LineSegment& ray, const CRegion& region,
  CastResult& result, set<const CRegion*>& visitedRegions,
  set<entityId_t>& visitedJoiningEdges) {

  visitedRegions.insert(&region);

  for (auto it = region.children.begin(); it != region.children.end(); ++it) {
    if (visitedRegions.find(it->get()) == visitedRegions.end()) {
      findIntersections_r(camera, ray, **it, result, visitedRegions, visitedJoiningEdges);
    }
  }

  Matrix invCamMatrix = camera.matrix().inverse();

  for (auto it = region.sprites.begin(); it != region.sprites.end(); ++it) {
    CSprite& sprite = **it;
    Point pos = invCamMatrix * sprite.pos;
    double w = sprite.size.x;
    LineSegment lseg(Point(pos.x, pos.y - 0.5 * w), Point(pos.x, pos.y + 0.5 * w));

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
    CEdge& edge = **it;

    LineSegment lseg = transform(edge.lseg, invCamMatrix);

    Point pt;
    if (lineSegmentIntersect(ray, lseg, pt)) {
      Intersection* X = constructIntersection(edge.kind);
      X->point_cam = pt;
      X->point_world = camera.matrix() * pt;
      X->distanceFromCamera = pt.x;
      X->distanceAlongTarget = distance(lseg.A, pt);

      if (edge.kind == CRenderKind::WALL) {
        WallX* wallX = dynamic_cast<WallX*>(X);
        CWall& wall = dynamic_cast<CWall&>(edge);

        wallX->wall = &wall;
        result.intersections.push_back(pIntersection_t(X));
      }
      else if (edge.kind == CRenderKind::JOINING_EDGE) {
        JoiningEdgeX* jeX = dynamic_cast<JoiningEdgeX*>(X);
        CJoiningEdge& je = dynamic_cast<CJoiningEdge&>(edge);

        jeX->joiningEdge = &je;

        const CRegion& next = je.regionA == &region ? *je.regionB : *je.regionA;

        auto pX = pIntersection_t(X);
        if (visitedJoiningEdges.find(je.joinId) == visitedJoiningEdges.end()) {
          result.intersections.push_back(std::move(pX));
          visitedJoiningEdges.insert(je.joinId);
        }

        if (visitedRegions.find(&next) == visitedRegions.end()) {
          findIntersections_r(camera, ray, next, result, visitedRegions, visitedJoiningEdges);
        }
      }
      else {
        EXCEPTION("Unexpected intersection type");
      }
    }
  }
}

//===========================================
// castRay
//===========================================
static void castRay(Vec2f r, const RenderGraph& rg, const Player& player,
  const CRegion& currentRegion, CastResult& result) {

  auto& intersections = result.intersections;

  const Camera& cam = player.camera();
  LineSegment ray(Point(0, 0), Point(r.x * 999.9, r.y * 999.9));

  set<const CRegion*> visitedRegions;
  set<entityId_t> visitedJoiningEdges;
  findIntersections_r(cam, ray, currentRegion, result, visitedRegions, visitedJoiningEdges);

  intersections.sort([](const pIntersection_t& a, const pIntersection_t& b) {
    return a->distanceFromCamera < b->distanceFromCamera;
  });

  LineSegment projPlane(Point(cam.F, -rg.viewport.y / 2),
    Point(cam.F, rg.viewport.y * 0.5));

  Matrix m(cam.vAngle, Vec2f(0, 0));
  LineSegment rotProjPlane = transform(projPlane, m);

  LineSegment projRay0(Point(0, 0), rotProjPlane.A * 999.9);
  LineSegment projRay1(Point(0, 0), rotProjPlane.B * 999.9);
  double subview0 = 0;
  double subview1 = rg.viewport.y;

  const CRegion* region = &currentRegion;
  int last = -1;
  for (auto it = intersections.begin(); it != intersections.end(); ++it) {
    ++last;

    if ((*it)->kind == IntersectionKind::WALL) {
      WallX& X = dynamic_cast<WallX&>(**it);

      double floorHeight = X.wall->region->floorHeight;
      double targetHeight = X.wall->region->ceilingHeight - X.wall->region->floorHeight;
      const Point& pt = X.point_cam;

      LineSegment wall(Point(pt.x, floorHeight - cam.height),
        Point(pt.x, floorHeight - cam.height + targetHeight));

      LineSegment wallRay0(Point(0, 0), Point(pt.x, wall.A.y));
      LineSegment wallRay1(Point(0, 0), Point(pt.x, wall.B.y));

      Point p = lineIntersect(wallRay0.line(), rotProjPlane.line());
      double proj_w0 = rotProjPlane.signedDistance(p.x);
      p = lineIntersect(wallRay1.line(), rotProjPlane.line());
      double proj_w1 = rotProjPlane.signedDistance(p.x);

      double wall_s0 = lineIntersect(projRay0.line(), wall.line()).y;
      double wall_s1 = lineIntersect(projRay1.line(), wall.line()).y;

      auto wallAClip = clipNumber(wall.A.y, Size(wall_s0, wall_s1), X.slice.sliceBottom_wd);
      auto wallBClip = clipNumber(wall.B.y, Size(wall_s0, wall_s1), X.slice.sliceTop_wd);

      if (wallAClip == CLIPPED_TO_BOTTOM) {
        proj_w0 = subview0;
      }
      if (wallAClip == CLIPPED_TO_TOP) {
        proj_w0 = subview1;
      }
      if (wallBClip == CLIPPED_TO_BOTTOM) {
        proj_w1 = subview0;
      }
      if (wallBClip == CLIPPED_TO_TOP) {
        proj_w1 = subview1;
      }

      X.slice.projSliceBottom_wd = proj_w0;
      X.slice.projSliceTop_wd = proj_w1;

      X.slice.viewportBottom_wd = subview0;
      X.slice.viewportTop_wd = subview1;

      break;
    }
    else if ((*it)->kind == IntersectionKind::JOINING_EDGE) {
      JoiningEdgeX& X = dynamic_cast<JoiningEdgeX&>(**it);

      // TODO: Fix
      //assert(region == X.joiningEdge->regionA || region == X.joiningEdge->regionB);
      CRegion* nextRegion = region == X.joiningEdge->regionA ?
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

      LineSegment bottomWall(Point(pt.x, bottomWallA), Point(pt.x, bottomWallB));
      LineSegment topWall(Point(pt.x, topWallA), Point(pt.x, topWallB));

      LineSegment bottomWallRay0(Point(0, 0), bottomWall.A);
      LineSegment bottomWallRay1(Point(0, 0), bottomWall.B);

      LineSegment topWallRay0(Point(0, 0), topWall.A);
      LineSegment topWallRay1(Point(0, 0), topWall.B);

      LineSegment wall(Point(pt.x, bottomWallA), Point(pt.x, topWallB));
      double wall_s0 = lineIntersect(projRay0.line(), wall.line()).y;
      double wall_s1 = lineIntersect(projRay1.line(), wall.line()).y;

      auto bWallAClip = clipNumber(bottomWall.A.y, Size(wall_s0, wall_s1), X.slice0.sliceBottom_wd);
      auto bWallBClip = clipNumber(bottomWall.B.y, Size(wall_s0, wall_s1), X.slice0.sliceTop_wd);

      auto tWallAClip = clipNumber(topWall.A.y, Size(wall_s0, wall_s1), X.slice1.sliceBottom_wd);
      auto tWallBClip = clipNumber(topWall.B.y, Size(wall_s0, wall_s1), X.slice1.sliceTop_wd);

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

      if (bWallAClip == CLIPPED_TO_BOTTOM) {
        proj_bw0 = subview0;
      }
      if (bWallAClip == CLIPPED_TO_TOP) {
        proj_bw0 = subview1;
      }
      if (bWallBClip == CLIPPED_TO_BOTTOM) {
        proj_bw1 = subview0;
        vw0 = rotProjPlane.A;
      }
      if (bWallBClip == CLIPPED_TO_TOP) {
        proj_bw1 = subview1;
        vw0 = rotProjPlane.B;
      }
      if (tWallAClip == CLIPPED_TO_BOTTOM) {
        proj_tw0 = subview0;
        vw1 = rotProjPlane.A;
      }
      if (tWallAClip == CLIPPED_TO_TOP) {
        proj_tw0 = subview1;
        vw1 = rotProjPlane.B;
      }
      if (tWallBClip == CLIPPED_TO_BOTTOM) {
        proj_tw1 = subview0;
      }
      if (tWallBClip == CLIPPED_TO_TOP) {
        proj_tw1 = subview1;
      }

      X.slice0.projSliceBottom_wd = proj_bw0;
      X.slice0.projSliceTop_wd = proj_bw1;
      X.slice1.projSliceBottom_wd = proj_tw0;
      X.slice1.projSliceTop_wd = proj_tw1;

      X.slice0.viewportBottom_wd = subview0;
      X.slice0.viewportTop_wd = subview1;
      X.slice1.viewportBottom_wd = subview0;
      X.slice1.viewportTop_wd = subview1;

      if (proj_bw1 > subview0) {
        subview0 = proj_bw1;
        projRay0 = LineSegment(Point(0, 0), vw0 * 999.9);
      }

      if (proj_tw0 < subview1) {
        subview1 = proj_tw0;
        projRay1 = LineSegment(Point(0, 0), vw1 * 999.9);
      }

      region = nextRegion;
    }
    else if ((*it)->kind == IntersectionKind::SPRITE) {
      SpriteX& X = dynamic_cast<SpriteX&>(**it);

      double floorHeight = region->floorHeight;
      const Point& pt = X.point_cam;

      LineSegment wall(Point(pt.x, floorHeight - cam.height),
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
// drawSkySlice
//===========================================
static void drawSkySlice(QImage& target, const RenderGraph& rg, const Player& player,
  const ScreenSlice& slice, int screenX_px) {

  const Camera& cam = player.camera();

  const Texture& skyTex = rg.textures.at("sky");
  Size texSz_px(skyTex.image.rect().width(), skyTex.image.rect().height());

  int W_px = target.rect().width();
  int H_px = target.rect().height();

  double hPxAngle = cam.hFov / W_px;
  double vPxAngle = cam.vFov / H_px;

  double hAngle = normaliseAngle(cam.angle - 0.5 * cam.hFov + hPxAngle * screenX_px);
  double s = hAngle / (2.0 * PI);
  assert(isBetween(s, 0.0, 1.0));

  int x = texSz_px.x * s;

  double maxPitch = DEG_TO_RAD(20.0); // TODO
  double minVAngle = -0.5 * cam.vFov - maxPitch;
  double maxVAngle = 0.5 * cam.vFov + maxPitch;
  double vAngleRange = maxVAngle - minVAngle;
  double viewportBottomAngle = cam.vAngle - 0.5 * cam.vFov;

  for (int j = slice.viewportTop_px; j <= slice.sliceTop_px; ++j) {
    double vAngle = viewportBottomAngle + (H_px - j) * vPxAngle;
    double s = 1.0 - normaliseAngle(vAngle - minVAngle) / vAngleRange;
    assert(isBetween(s, 0.0, 1.0));

    QRgb* pixels = reinterpret_cast<QRgb*>(target.scanLine(j));
    pixels[screenX_px] = skyTex.image.pixel(x, s * texSz_px.y);
  }
}

//===========================================
// drawCeilingSlice
//===========================================
static void drawCeilingSlice(QImage& target, const RenderGraph& rg, const Player& player,
  const CRegion* region, const Point& collisionPoint, const ScreenSlice& slice, int screenX_px,
  double projX_wd, double vWorldUnit_px, const tanMap_t& tanMap_rp, const atanMap_t& atanMap) {

  double screenH_px = rg.viewport.y * vWorldUnit_px;
  const Camera& cam = player.camera();
  const Texture& ceilingTex = rg.textures.at(region->ceilingTexture);

  double hAngle = atan(projX_wd / cam.F);
  LineSegment ray(cam.pos, collisionPoint);

  Size texSz_px(ceilingTex.image.rect().width(), ceilingTex.image.rect().height());
  Size texSz_wd_rp(1.0 / ceilingTex.size_wd.x, 1.0 / ceilingTex.size_wd.y);

  double vWorldUnit_px_rp = 1.0 / vWorldUnit_px;
  double F_rp = 1.0 / cam.F;
  double rayLen_rp = 1.0 / ray.length();
  double cosHAngle_rp = 1.0 / cos(hAngle);

  for (int j = slice.sliceTop_px; j >= slice.viewportTop_px; --j) {
    double projY_wd = (screenH_px * 0.5 - j) * vWorldUnit_px_rp;
    double vAngle = fastATan(atanMap, projY_wd * F_rp) + cam.vAngle;
    double d_ = (region->ceilingHeight - cam.height) * fastTan_rp(tanMap_rp, vAngle);
    double d = d_ * cosHAngle_rp;
    double s = d * rayLen_rp;
    Point p(ray.A.x + (ray.B.x - ray.A.x) * s, ray.A.y + (ray.B.y - ray.A.y) * s);

    Point texel = worldPointToFloorTexel(p, texSz_wd_rp, texSz_px);

    QRgb* pixels = reinterpret_cast<QRgb*>(target.scanLine(j));
    pixels[screenX_px] = ceilingTex.image.pixel(texel.x, texel.y);
  }
}

//===========================================
// getFloorDecal
//
// x is set to the point inside decal space
//===========================================
static const CFloorDecal* getFloorDecal(const CRegion* region, const Point& pt, Point& x) {
  for (auto it = region->floorDecals.begin(); it != region->floorDecals.end(); ++it) {
    const CFloorDecal& decal = **it;

    x = decal.transform.inverse() * pt;

    if (isBetween(x.x, 0, decal.size.x)
      && isBetween(x.y, 0, decal.size.y)) {

      return &decal;
    }
  }

  return nullptr;
}

//===========================================
// drawFloorSlice
//===========================================
static void drawFloorSlice(QImage& target, const RenderGraph& rg, const Player& player,
  const CRegion* region, const Point& collisionPoint, const ScreenSlice& slice, int screenX_px,
  double projX_wd, double vWorldUnit_px, const tanMap_t& tanMap_rp, const atanMap_t& atanMap) {

  const Camera& cam = player.camera();

  double screenH_px = rg.viewport.y * vWorldUnit_px;
  const Texture& floorTex = rg.textures.at(region->floorTexture);

  double hAngle = atan(projX_wd / cam.F);
  LineSegment ray(cam.pos, collisionPoint);

  Size texSz_px(floorTex.image.rect().width(), floorTex.image.rect().height());
  Size texSz_wd_rp(1.0 / floorTex.size_wd.x, 1.0 / floorTex.size_wd.y);

  double vWorldUnit_px_rp = 1.0 / vWorldUnit_px;
  double F_rp = 1.0 / cam.F;
  double rayLen_rp = 1.0 / ray.length();
  double cosHAngle_rp = 1.0 / cos(hAngle);

  for (int j = slice.sliceBottom_px; j < slice.viewportBottom_px; ++j) {
    double projY_wd = (j - screenH_px * 0.5) * vWorldUnit_px_rp;
    double vAngle = fastATan(atanMap, projY_wd * F_rp) - cam.vAngle;
    double d_ = (cam.height - region->floorHeight) * fastTan_rp(tanMap_rp, vAngle);
    double d = d_ * cosHAngle_rp;
    double s = d * rayLen_rp;
    Point p(ray.A.x + (ray.B.x - ray.A.x) * s, ray.A.y + (ray.B.y - ray.A.y) * s);

    Point decalPt;
    const CFloorDecal* decal = getFloorDecal(region, p, decalPt);

    if (decal != nullptr) {
      const Texture& decalTex = rg.textures.at(decal->texture);
      Size texSz_px(decalTex.image.rect().width(), decalTex.image.rect().height());

      Point texel(texSz_px.x * decalPt.x / decal->size.x, texSz_px.y * decalPt.y / decal->size.y);
      QRgb* pixels = reinterpret_cast<QRgb*>(target.scanLine(j));
      pixels[screenX_px] = decalTex.image.pixel(texel.x, texel.y);
    }
    else {
      Point texel = worldPointToFloorTexel(p, texSz_wd_rp, texSz_px);
      QRgb* pixels = reinterpret_cast<QRgb*>(target.scanLine(j));
      pixels[screenX_px] = floorTex.image.pixel(texel.x, texel.y);
    }
  }
}

//===========================================
// sampleWallTexture
//===========================================
static void sampleWallTexture(const QRect& texRect, double camHeight_wd, const Size& viewport_px,
  double screenX_px, double hWorldUnit_px, double vWorldUnit_px, double targetH_wd,
  double distanceAlongTarget, const Slice& slice, const Size& texSz_wd, vector<QRectF>& trgRects,
  vector<QRect>& srcRects) {

  double H_tx = texRect.height();
  double W_tx = texRect.width();

  double projSliceH_wd = slice.projSliceTop_wd - slice.projSliceBottom_wd;
  double sliceH_wd = slice.sliceTop_wd - slice.sliceBottom_wd;
  double sliceToProjScale = projSliceH_wd / sliceH_wd;
  double projTexH_wd = texSz_wd.y * sliceToProjScale;

  // World space
  double sliceBottom_wd = slice.sliceBottom_wd + camHeight_wd - targetH_wd;
  double sliceTop_wd = slice.sliceTop_wd + camHeight_wd;

  // World space (not camera space)
  auto fnSliceToProjY = [&](double y) {
    return slice.projSliceBottom_wd + (y - sliceBottom_wd) * sliceToProjScale;
  };

  double hWorldUnit_tx = W_tx / texSz_wd.x;
  double vWorldUnit_tx = H_tx / texSz_wd.y;

  auto fnProjToScreenY = [&](double y) {
    return viewport_px.y - (y * vWorldUnit_px);
  };

  double nx = distanceAlongTarget / texSz_wd.x;
  double x = (nx - floor(nx)) * texSz_wd.x;

  int j0 = floor(sliceBottom_wd / texSz_wd.y);
  int j1 = ceil(sliceTop_wd / texSz_wd.y);

  double bottomOffset_wd = sliceBottom_wd - j0 * texSz_wd.y;
  double topOffset_wd = j1 * texSz_wd.y - sliceTop_wd;

  for (int j = j0; j < j1; ++j) {
    QRect srcRect;
    srcRect.setX(texRect.x() + x * hWorldUnit_tx);
    srcRect.setY(texRect.y());
    srcRect.setWidth(1);
    srcRect.setHeight(texRect.height());

    double y = j * texSz_wd.y;

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
static ScreenSlice drawSlice(QPainter& painter, const RenderGraph& rg, const Player& player,
  double F, double distanceAlongTarget, const Slice& slice, const string& texture,
  double screenX_px, const Size& viewport_px, double targetH_wd = 0) {

  double hWorldUnit_px = viewport_px.x / rg.viewport.x;
  double vWorldUnit_px = viewport_px.y / rg.viewport.y;

  const Texture& wallTex = rg.textures.at(texture);

  int screenSliceBottom_px = viewport_px.y - slice.projSliceBottom_wd * vWorldUnit_px;
  int screenSliceTop_px = viewport_px.y - slice.projSliceTop_wd * vWorldUnit_px;

  if (screenSliceBottom_px - screenSliceTop_px > 0) {
    vector<QRect> srcRects;
    vector<QRectF> trgRects;
    sampleWallTexture(wallTex.image.rect(), player.camera().height, viewport_px, screenX_px,
      hWorldUnit_px, vWorldUnit_px, targetH_wd, distanceAlongTarget, slice, wallTex.size_wd,
      trgRects, srcRects);

    assert(srcRects.size() == trgRects.size());

    for (unsigned int i = 0; i < srcRects.size(); ++i) {
      painter.drawImage(trgRects[i], wallTex.image, srcRects[i]);
    }
  }

  int viewportBottom_px = (rg.viewport.y - slice.viewportBottom_wd) * vWorldUnit_px;
  int viewportTop_px = (rg.viewport.y - slice.viewportTop_wd) * vWorldUnit_px;

  return ScreenSlice{screenSliceBottom_px, screenSliceTop_px, viewportBottom_px, viewportTop_px};
}

//===========================================
// drawSprite
//===========================================
static void drawSprite(QPainter& painter, const RenderGraph& rg, const Player& player,
  const Size& viewport_px, const SpriteX& spriteX, double screenX_px) {

  const Camera& cam = player.camera();

  double vWorldUnit_px = viewport_px.y / rg.viewport.y;

  const CSprite& sprite = *spriteX.sprite;
  const Slice& slice = spriteX.slice;

  const Texture& tex = rg.textures.at(sprite.texture);
  const QRectF& uv = sprite.textureRegion(cam.pos);
  QRect r = tex.image.rect();
  QRect frame(r.width() * uv.x(), r.height() * uv.y(), r.width() * uv.width(),
    r.height() * uv.height());

  int screenSliceBottom_px = viewport_px.y - slice.projSliceBottom_wd * vWorldUnit_px;
  int screenSliceTop_px = viewport_px.y - slice.projSliceTop_wd * vWorldUnit_px;

  QRect srcRect = sampleSpriteTexture(frame, spriteX, cam.height, sprite.size.x,
    sprite.size.y, sprite.region->floorHeight);
  QRect trgRect(screenX_px, screenSliceTop_px, 1, screenSliceBottom_px - screenSliceTop_px);

  painter.drawImage(trgRect, tex.image, srcRect);
}

//===========================================
// drawWallDecal
//===========================================
static void drawWallDecal(QPainter& painter, const RenderGraph& rg, const CWallDecal& decal,
  const WallX& wallX, int screenX_px, const Size& viewport_px, double camHeight,
  double vWorldUnit_px) {

  const Texture& decalTex = rg.textures.at(decal.texture);

  double projSliceH_wd = wallX.slice.projSliceTop_wd - wallX.slice.projSliceBottom_wd;
  double sliceH_wd = wallX.slice.sliceTop_wd - wallX.slice.sliceBottom_wd;
  double sliceToProjScale = projSliceH_wd / sliceH_wd;

  // World space
  double sliceBottom_wd = wallX.slice.sliceBottom_wd + camHeight;

  // World space (not camera space)
  auto fnSliceToProjY = [&](double y) {
    return wallX.slice.projSliceBottom_wd + (y - sliceBottom_wd) * sliceToProjScale;
  };

  auto fnProjToScreenY = [&](double y) {
    return viewport_px.y - (y * vWorldUnit_px);
  };

  double floorH = wallX.wall->region->floorHeight;

  double x = wallX.distanceAlongTarget - decal.pos.x;
  int i = decalTex.image.width() * x / decal.size.x;

  double y0 = floorH + decal.pos.y;
  double y1 = floorH + decal.pos.y + decal.size.y;

  int y0_px = fnProjToScreenY(fnSliceToProjY(y0));
  int y1_px = fnProjToScreenY(fnSliceToProjY(y1));

  QRect srcRect(i, 0, 1, decalTex.image.height());
  QRect trgRect(screenX_px, y1_px, 1, y0_px - y1_px);

  painter.drawImage(trgRect, decalTex.image, srcRect);
}

//===========================================
// Renderer::getWallDecal
//===========================================
static CWallDecal* getWallDecal(const CWall& wall, double x) {
  for (auto it = wall.decals.begin(); it != wall.decals.end(); ++it) {
    CWallDecal* decal = it->get();

    double x0 = decal->pos.x;
    double x1 = decal->pos.x + decal->size.x;

    if (isBetween(x, x0, x1)) {
      return decal;
    }
  }

  return nullptr;
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
void Renderer::renderScene(QImage& target, const Player& player) {
  QPainter painter;
  painter.begin(&target);

  Size viewport_px(target.width(), target.height());
  const Camera& cam = player.camera();

  double hWorldUnit_px = viewport_px.x / rg.viewport.x;
  double vWorldUnit_px = viewport_px.y / rg.viewport.y;

  QRect rect(QPoint(), QSize(viewport_px.x, viewport_px.y));
  painter.fillRect(rect, QBrush(QColor(0, 0, 0)));

  for (int screenX_px = 0; screenX_px < viewport_px.x; ++screenX_px) {
    double projX_wd = static_cast<double>(screenX_px - viewport_px.x / 2) / hWorldUnit_px;

    CastResult result;
    castRay(Vec2f(cam.F, projX_wd), rg, player, region(player.currentRegion), result);

    for (auto it = result.intersections.rbegin(); it != result.intersections.rend(); ++it) {
      Intersection& X = **it;

      if (X.kind == IntersectionKind::WALL) {
        const WallX& wallX = dynamic_cast<const WallX&>(X);

        ScreenSlice slice = drawSlice(painter, rg, player, cam.F, wallX.distanceAlongTarget,
          wallX.slice, wallX.wall->texture, screenX_px, viewport_px);

        CWallDecal* decal = getWallDecal(*wallX.wall, wallX.distanceAlongTarget);
        if (decal != nullptr) {
          drawWallDecal(painter, rg, *decal, wallX, screenX_px, viewport_px, cam.height,
            vWorldUnit_px);
        }

        drawFloorSlice(target, rg, player, wallX.wall->region, wallX.point_world, slice, screenX_px,
          projX_wd, vWorldUnit_px, m_tanMap_rp, m_atanMap);

        if (wallX.wall->region->hasCeiling) {
          drawCeilingSlice(target, rg, player, wallX.wall->region, wallX.point_world, slice, screenX_px,
            projX_wd, vWorldUnit_px, m_tanMap_rp, m_atanMap);
        }
        else {
          drawSkySlice(target, rg, player, slice, screenX_px);
        }
      }
      else if (X.kind == IntersectionKind::JOINING_EDGE) {
        const JoiningEdgeX& jeX = dynamic_cast<const JoiningEdgeX&>(X);

        ScreenSlice slice0 = drawSlice(painter, rg, player, cam.F, jeX.distanceAlongTarget, jeX.slice0,
          jeX.joiningEdge->bottomTexture, screenX_px, viewport_px);

        drawFloorSlice(target, rg, player, jeX.nearRegion, jeX.point_world, slice0,
          screenX_px, projX_wd, vWorldUnit_px, m_tanMap_rp, m_atanMap);

        // TODO: Make use of jeX.farRegion->ceilingHeight optional
        ScreenSlice slice1 = drawSlice(painter, rg, player, cam.F, jeX.distanceAlongTarget, jeX.slice1,
          jeX.joiningEdge->topTexture, screenX_px, viewport_px, jeX.farRegion->ceilingHeight);

        if (jeX.nearRegion->hasCeiling) {
          drawCeilingSlice(target, rg, player, jeX.nearRegion, jeX.point_world, slice1, screenX_px,
            projX_wd, vWorldUnit_px, m_tanMap_rp, m_atanMap);
        }
        else {
          drawSkySlice(target, rg, player, slice1, screenX_px);
        }
      }
      else if (X.kind == IntersectionKind::SPRITE) {
        const SpriteX& spriteX = dynamic_cast<const SpriteX&>(X);
        drawSprite(painter, rg, player, viewport_px, spriteX, screenX_px);
      }
    }
  }

  painter.end();
}

//===========================================
// Renderer::update
//===========================================
void Renderer::update() {

}

//===========================================
// Renderer::handleEvent
//===========================================
void Renderer::handleEvent(const GameEvent& event) {

}

//===========================================
// similar
//===========================================
static bool similar(const LineSegment& l1, const LineSegment& l2) {
  double delta = 4.0;
  return (distance(l1.A, l2.A) <= delta && distance(l1.B, l2.B) <= delta)
    || (distance(l1.A, l2.B) <= delta && distance(l1.B, l2.A) <= delta);
}

//===========================================
// areTwins
//===========================================
static bool areTwins(const CJoiningEdge& je1, const CJoiningEdge& je2) {
  return similar(je1.lseg, je2.lseg);
}

//===========================================
// connectSubregions_r
//===========================================
static void connectSubregions_r(CRegion& region) {
  if (region.children.size() == 0) {
    return;
  }

  for (auto it = region.children.begin(); it != region.children.end(); ++it) {
    CRegion& r = **it;
    connectSubregions_r(r);

    for (auto jt = r.edges.begin(); jt != r.edges.end(); ++jt) {
      if ((*jt)->kind == CRenderKind::JOINING_EDGE) {
        CJoiningEdge* je = dynamic_cast<CJoiningEdge*>(*jt);
        assert(je != nullptr);

        bool hasTwin = false;
        forEachCRegion(region, [&](CRegion& r_) {
          if (!hasTwin) {
            if (&r_ != &r) {
              for (auto lt = r_.edges.begin(); lt != r_.edges.end(); ++lt) {
                if ((*lt)->kind == CRenderKind::JOINING_EDGE) {
                  CJoiningEdge* other = dynamic_cast<CJoiningEdge*>(*lt);
                  assert(other != nullptr);

                  if (je == other) {
                    hasTwin = true;
                    break;
                  }

                  if (areTwins(*je, *other)) {
                    hasTwin = true;

                    je->joinId = other->joinId;
                    je->regionA = other->regionA = &r;
                    je->regionB = other->regionB = &r_;

                    je->mergeIn(*other);
                    other->mergeIn(*je);

                    break;
                  }
                }
              }
            }
          }
        });

        if (!hasTwin) {
          je->regionA = &r;
          je->regionB = &region;
        }
      }
    }
  };
}

//===========================================
// Renderer::connectRegions
//===========================================
void Renderer::connectRegions() {
  connectSubregions_r(*rg.rootRegion);
}

//===========================================
// addToRegion
//===========================================
static void addToRegion(RenderGraph& rg, CRegion& region, pCRender_t child) {
  switch (child->kind) {
    case CRenderKind::REGION: {
      pCRegion_t ptr(dynamic_cast<CRegion*>(child.release()));
      ptr->parent = &region;
      region.children.push_back(std::move(ptr));
      break;
    }
    case CRenderKind::JOINING_EDGE:
    case CRenderKind::WALL: {
      pCEdge_t ptr(dynamic_cast<CEdge*>(child.release()));
      region.edges.push_back(ptr.get());
      rg.edges.push_back(std::move(ptr));
      break;
    }
    case CRenderKind::FLOOR_DECAL: {
      pCFloorDecal_t ptr(dynamic_cast<CFloorDecal*>(child.release()));
      region.floorDecals.push_back(std::move(ptr));
      break;
    }
    case CRenderKind::SPRITE: {
      pCSprite_t ptr(dynamic_cast<CSprite*>(child.release()));
      region.sprites.push_back(std::move(ptr));
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to region");
  }
}

//===========================================
// addToWall
//===========================================
static void addToWall(CWall& edge, pCRender_t child) {
  switch (child->kind) {
    case CRenderKind::WALL_DECAL: {
      pCWallDecal_t ptr(dynamic_cast<CWallDecal*>(child.release()));
      edge.decals.push_back(std::move(ptr));
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to Wall");
  }
}

//===========================================
// addChildToComponent
//===========================================
static void addChildToComponent(RenderGraph& rg, CRender& parent, pCRender_t child) {
  switch (parent.kind) {
    case CRenderKind::REGION:
      addToRegion(rg, dynamic_cast<CRegion&>(parent), std::move(child));
      break;
    case CRenderKind::WALL:
      addToWall(dynamic_cast<CWall&>(parent), std::move(child));
      break;
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to component of kind "
        << parent.kind);
  };
}

//===========================================
// removeFromRegion
//===========================================
static void removeFromRegion(RenderGraph& rg, CRegion& region, const CRender& child) {
  switch (child.kind) {
    case CRenderKind::REGION: {
      region.children.remove_if([&](const pCRegion_t& e) {
        return e.get() == dynamic_cast<const CRegion*>(&child);
      });
      break;
    }
    case CRenderKind::JOINING_EDGE:
    case CRenderKind::WALL: {
      region.edges.remove_if([&](const CEdge* e) {
        return e == dynamic_cast<const CEdge*>(&child);
      });
      rg.edges.remove_if([&](const pCEdge_t& e) {
        return e.get() == dynamic_cast<const CEdge*>(&child);
      });
      break;
    }
    case CRenderKind::FLOOR_DECAL: {
      region.floorDecals.remove_if([&](const pCFloorDecal_t& e) {
        return e.get() == dynamic_cast<const CFloorDecal*>(&child);
      });
      break;
    }
    case CRenderKind::SPRITE: {
      region.sprites.remove_if([&](const pCSprite_t& e) {
        return e.get() == dynamic_cast<const CSprite*>(&child);
      });
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child.kind << " to region");
  }
}

//===========================================
// removeFromWall
//===========================================
static void removeFromWall(CWall& edge, const CRender& child) {
  switch (child.kind) {
    case CRenderKind::WALL_DECAL: {
      edge.decals.remove_if([&](const pCWallDecal_t& e) {
        return e.get() == dynamic_cast<const CWallDecal*>(&child);
      });
      break;
    }
    default:
      EXCEPTION("Cannot remove component of kind " << child.kind << " from Wall");
  }
}

//===========================================
// removeChildFromComponent
//===========================================
static void removeChildFromComponent(RenderGraph& rg, CRender& parent,
  const CRender& child) {

  switch (parent.kind) {
    case CRenderKind::REGION:
      removeFromRegion(rg, dynamic_cast<CRegion&>(parent), child);
      break;
    case CRenderKind::WALL:
      removeFromWall(dynamic_cast<CWall&>(parent), child);
      break;
    default:
      EXCEPTION("Cannot remove component of kind " << child.kind << " from component of kind "
        << parent.kind);
  };
}

//===========================================
// Renderer::addComponent
//===========================================
void Renderer::addComponent(pComponent_t component) {
  if (component->kind() != ComponentKind::C_RENDER) {
    EXCEPTION("Component is not of kind C_RENDER");
  }

  CRender* ptr = dynamic_cast<CRender*>(component.release());
  pCRender_t c(ptr);

  if (c->parentId == -1) {
    if (rg.rootRegion) {
      EXCEPTION("Root region already set");
    }

    if (c->kind != CRenderKind::REGION) {
      EXCEPTION("Component has no parent; Only regions can be root");
    }

    pCRegion_t z(dynamic_cast<CRegion*>(c.release()));

    rg.rootRegion = std::move(z);
    m_components.clear();
  }
  else {
    auto it = m_components.find(c->parentId);
    if (it == m_components.end()) {
      EXCEPTION("Could not find parent component with id " << c->parentId);
    }

    CRender* parent = it->second;
    assert(parent->entityId() == c->parentId);

    m_entityChildren[c->parentId].insert(c->entityId());
    addChildToComponent(rg, *parent, std::move(c));
  }

  m_components.insert(std::make_pair(ptr->entityId(), ptr));
}

//===========================================
// Renderer::isRoot
//===========================================
bool Renderer::isRoot(const CRender& c) const {
  if (c.kind != CRenderKind::REGION) {
    return false;
  }
  if (rg.rootRegion == nullptr) {
    return false;
  }
  const CRegion* ptr = dynamic_cast<const CRegion*>(&c);
  return ptr == rg.rootRegion.get();
}

//===========================================
// Renderer::removeEntity_r
//===========================================
void Renderer::removeEntity_r(entityId_t id) {
  m_components.erase(id);

  auto it = m_entityChildren.find(id);
  if (it != m_entityChildren.end()) {
    set<entityId_t>& children = it->second;

    for (auto jt = children.begin(); jt != children.end(); ++jt) {
      removeEntity_r(*jt);
    }
  }

  m_entityChildren.erase(id);
}

//===========================================
// Renderer::removeEntity
//===========================================
void Renderer::removeEntity(entityId_t id) {
  auto it = m_components.find(id);
  if (it == m_components.end()) {
    return;
  }

  CRender& c = *it->second;
  auto jt = m_components.find(c.parentId);

  if (jt != m_components.end()) {
    CRender& parent = *jt->second;
    removeChildFromComponent(rg, parent, c);
  }
  else {
    assert(isRoot(c));
  }

  removeEntity_r(id);
}
