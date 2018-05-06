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
#include "raycast/renderer.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/entity_manager.hpp"
#include "exception.hpp"


using std::string;
using std::list;
using std::set;
using std::vector;
using std::array;
using std::unique_ptr;
using std::function;
using std::for_each;


static const double TWO_FIVE_FIVE_RP = 1.0 / 255.0;


static inline CVRect& getVRect(const SpatialSystem& spatialSystem, const CWallDecal& d) {
  return dynamic_cast<CVRect&>(spatialSystem.getComponent(d.entityId()));
}

static inline CHRect& getHRect(const SpatialSystem& spatialSystem, const CFloorDecal& d) {
  return dynamic_cast<CHRect&>(spatialSystem.getComponent(d.entityId()));
}

//===========================================
// forEachConstCRegion
//===========================================
void forEachConstCRegion(const CRegion& region, function<void(const CRegion&)> fn) {
  fn(region);
  for_each(region.children.begin(), region.children.end(), [&](const unique_ptr<CRegion>& r) {
    forEachConstCRegion(*r, fn);
  });
}

//===========================================
// forEachCRegion
//===========================================
void forEachCRegion(CRegion& region, function<void(CRegion&)> fn) {
  fn(region);
  for_each(region.children.begin(), region.children.end(), [&](unique_ptr<CRegion>& r) {
    forEachCRegion(*r, fn);
  });
}

//===========================================
// blend
//===========================================
static inline QRgb blend(const QRgb& A, const QRgb& B, double alphaB) {
  double alphaA = 1.0 - alphaB;
  return qRgba(alphaA * qRed(A) + alphaB * qRed(B),
    alphaA * qGreen(A) + alphaB * qGreen(B),
    alphaA * qBlue(A) + alphaB * qBlue(B),
    255);
}

//===========================================
// pixel
//===========================================
static inline QRgb pixel(const QImage& img, int x, int y) {
  if (!isBetween(x, 0, img.width()) || !isBetween(y, 0, img.height())) {
    DBG_PRINT("Warning: Image coords (" << x << ", " << y << ") outside of range " << img.width()
      << "x" << img.height() << "\n")

    return 0;
  }
  return reinterpret_cast<const QRgb*>(img.constScanLine(y))[x];
}

//===========================================
// getShade
//===========================================
static inline double getShade(double distance) {
  static const Range r(0, 255);
  return clipNumber(distance * 0.2, r);
}

//===========================================
// applyShade
//===========================================
static inline QRgb applyShade(const QRgb& c, double distance) {
  double s = 1.0 - getShade(distance) * TWO_FIVE_FIVE_RP;
  return qRgba(qRed(c) * s, qGreen(c) * s, qBlue(c) * s, qAlpha(c));
}

//===========================================
// Renderer::drawImage
//===========================================
void Renderer::drawImage(const QRect& trgRect, const QImage& tex, const QRect& srcRect,
  double distance) const {

  int y0 = clipNumber(trgRect.y(), Range(0, m_target.height()));
  int y1 = clipNumber(trgRect.y() + trgRect.height(), Range(0, m_target.height()));
  int x0 = clipNumber(trgRect.x(), Range(0, m_target.width()));
  int x1 = clipNumber(trgRect.x() + trgRect.width(), Range(0, m_target.width()));

  double trgW_rp = 1.0 / trgRect.width();
  double trgH_rp = 1.0 / trgRect.height();

  for (int j = y0; j < y1; ++j) {
    QRgb* pixels = reinterpret_cast<QRgb*>(m_target.scanLine(j));
    double y = static_cast<double>(j - trgRect.y()) * trgH_rp;
    int srcY = srcRect.y() + y * srcRect.height();

    for (int i = x0; i < x1; ++i) {
      double x = static_cast<double>(i - trgRect.x()) * trgW_rp;
      int srcX = srcRect.x() + x * srcRect.width();

      QRgb srcPx = pixel(tex, srcX, srcY);
      double srcAlpha = qAlpha(srcPx);

      if (srcAlpha > 0) {
        pixels[i] = applyShade(blend(pixels[i], srcPx, TWO_FIVE_FIVE_RP * srcAlpha), distance);
      }
    }
  }
}

//===========================================
// Renderer::constructXWrapper
//===========================================
Renderer::XWrapper* Renderer::constructXWrapper(const SpatialSystem& spatialSystem,
  const RenderSystem& renderSystem, pIntersection_t X) const {

  Intersection* pX = X.get();

  switch (pX->kind) {
    case CSpatialKind::HARD_EDGE: {
      WallX* wrapper = new WallX(std::move(X));
      wrapper->hardEdge = dynamic_cast<const CHardEdge*>(&spatialSystem.getComponent(pX->entityId));
      wrapper->wall = dynamic_cast<const CWall*>(&renderSystem.getComponent(pX->entityId));
      return wrapper;
    }
    case CSpatialKind::SOFT_EDGE: {
      JoinX* wrapper = new JoinX(std::move(X));
      wrapper->softEdge = dynamic_cast<const CSoftEdge*>(&spatialSystem.getComponent(pX->entityId));
      wrapper->join = dynamic_cast<const CJoin*>(&renderSystem.getComponent(pX->entityId));
      return wrapper;
    }
    case CSpatialKind::V_RECT: {
      if (pX->parentKind == CSpatialKind::ZONE) {
        SpriteX* wrapper = new SpriteX(std::move(X));
        wrapper->vRect = dynamic_cast<const CVRect*>(&spatialSystem.getComponent(pX->entityId));
        wrapper->sprite = dynamic_cast<const CSprite*>(&renderSystem.getComponent(pX->entityId));
        return wrapper;
      }
    }
    default: break;
  }

  return nullptr;
}

//===========================================
// projectionPlane
//===========================================
static LineSegment projectionPlane(const Camera& cam, const RenderGraph& rg) {
  LineSegment lseg(Point(cam.F, -rg.viewport.y * 0.5), Point(cam.F, rg.viewport.y * 0.5));
  Matrix m(cam.vAngle, Vec2f(0, 0));

  return transform(lseg, m);
}

//===========================================
// Renderer::computeSlice
//===========================================
Renderer::Slice Renderer::computeSlice(const LineSegment& rotProjPlane, const LineSegment& wall,
  double subview0, double subview1, const LineSegment& projRay0, const LineSegment& projRay1,
  Point& projX0, Point& projX1) const {

  Slice slice;

  LineSegment wallRay0(Point(0, 0), wall.A);
  LineSegment wallRay1(Point(0, 0), wall.B);

  Line l = LineSegment(Point(wall.A.x, 0), Point(wall.A.x, 1)).line();
  double wall_s0 = lineIntersect(projRay0.line(), l).y;
  double wall_s1 = lineIntersect(projRay1.line(), l).y;

  auto wallAClip = clipNumber(wall.A.y, Range(wall_s0, wall_s1), slice.sliceBottom_wd);
  auto wallBClip = clipNumber(wall.B.y, Range(wall_s0, wall_s1), slice.sliceTop_wd);

  projX0 = lineIntersect(wallRay0.line(), rotProjPlane.line());
  projX0 = clipToLineSegment(projX0, rotProjPlane);
  double projW0 = rotProjPlane.signedDistance(projX0);

  projX1 = lineIntersect(wallRay1.line(), rotProjPlane.line());
  projX1 = clipToLineSegment(projX1, rotProjPlane);
  double projW1 = rotProjPlane.signedDistance(projX1);

  if (wallAClip == CLIPPED_TO_BOTTOM) {
    projW0 = subview0;
  }
  if (wallAClip == CLIPPED_TO_TOP) {
    projW0 = subview1;
  }
  if (wallBClip == CLIPPED_TO_BOTTOM) {
    projW1 = subview0;
  }
  if (wallBClip == CLIPPED_TO_TOP) {
    projW1 = subview1;
  }

  slice.projSliceBottom_wd = projW0;
  slice.projSliceTop_wd = projW1;
  slice.viewportBottom_wd = subview0;
  slice.viewportTop_wd = subview1;

  return slice;
}

//===========================================
// Renderer::castRay
//===========================================
void Renderer::castRay(const RenderGraph& rg, const Camera& cam, const SpatialSystem& spatialSystem,
  const RenderSystem& renderSystem, const Vec2f& dir, CastResult& result) const {

  LineSegment rotProjPlane = projectionPlane(cam, rg);

  list<pIntersection_t> intersections = spatialSystem.entitiesAlongRay(dir);

  LineSegment projRay0(Point(0, 0), rotProjPlane.A * 9999.9);
  LineSegment projRay1(Point(0, 0), rotProjPlane.B * 9999.9);
  double subview0 = 0;
  double subview1 = rg.viewport.y;

  const CZone* zone = &cam.zone();
  int last = -1;
  for (auto it = intersections.begin(); it != intersections.end(); ++it) {
    if (!renderSystem.hasComponent((*it)->entityId)) {
      continue;
    }

    ++last;
    XWrapper* X = constructXWrapper(spatialSystem, renderSystem, std::move(*it));
    if (X == nullptr) {
      continue;
    }
    result.intersections.push_back(pXWrapper_t(X));

    if (X->kind == XWrapperKind::WALL) {
      WallX& wallX = dynamic_cast<WallX&>(*X);
      wallX.nearZone = zone;

      double floorHeight = zone->floorHeight;
      double ceilingHeight = zone->hasCeiling ? zone->ceilingHeight
        : wallX.hardEdge->zone->ceilingHeight;
      double targetHeight = ceilingHeight - floorHeight;
      const Point& pt = wallX.X->point_rel;

      LineSegment wall(Point(pt.x, floorHeight - cam.height),
        Point(pt.x, floorHeight - cam.height + targetHeight));

      Point projX0, projX1;
      wallX.slice = computeSlice(rotProjPlane, wall, subview0, subview1, projRay0, projRay1,
        projX0, projX1);

      break;
    }
    else if (X->kind == XWrapperKind::JOIN) {
      JoinX& joinX = dynamic_cast<JoinX&>(*X);

      if(!(zone == joinX.softEdge->zoneA || zone == joinX.softEdge->zoneB)) {
        //DBG_PRINT("Warning: Possibly overlapping regions\n");
      }

      CZone* nextZone = zone == joinX.softEdge->zoneA ? joinX.softEdge->zoneB
        : joinX.softEdge->zoneA;

      joinX.nearZone = zone;
      joinX.farZone = nextZone;

      const CRegion& region =
        dynamic_cast<const CRegion&>(renderSystem.getComponent(zone->entityId()));
      const CRegion& nextRegion =
        dynamic_cast<const CRegion&>(renderSystem.getComponent(nextZone->entityId()));

      double floorDiff = nextZone->floorHeight - zone->floorHeight;
      double ceilingDiff = zone->ceilingHeight - nextZone->ceilingHeight;
      double nextZoneSpan = nextZone->ceilingHeight - nextZone->floorHeight;

      double bottomWallA = zone->floorHeight - cam.height;
      double bottomWallB = bottomWallA + floorDiff;
      double topWallA = bottomWallB + nextZoneSpan;
      double topWallB = topWallA + ceilingDiff;

      bool slice1Visible = true;

      if (floorDiff < 0) {
        bottomWallB = bottomWallA;
      }
      if (!nextRegion.hasCeiling || ceilingDiff < 0) {
        topWallA = topWallB;

        if (!region.hasCeiling) {
          slice1Visible = false;
        }
      }

      // Top and bottom walls cannot overlap
      topWallA = largest(topWallA, bottomWallB);
      bottomWallB = smallest(bottomWallB, topWallA);

      const Point& pt = joinX.X->point_rel;

      LineSegment bottomWall(Point(pt.x, bottomWallA), Point(pt.x, bottomWallB));
      LineSegment topWall(Point(pt.x, topWallA), Point(pt.x, topWallB));

      Point bProjX0, bProjX1, tProjX0, tProjX1;
      joinX.slice0 = computeSlice(rotProjPlane, bottomWall, subview0, subview1, projRay0, projRay1,
        bProjX0, bProjX1);
      joinX.slice1 = computeSlice(rotProjPlane, topWall, subview0, subview1, projRay0, projRay1,
        tProjX0, tProjX1);
      joinX.slice1.visible = slice1Visible;

      if (joinX.slice0.projSliceTop_wd > subview0) {
        subview0 = joinX.slice0.projSliceTop_wd;
        projRay0 = LineSegment(Point(0, 0), bProjX1 * 9999.9);
      }

      if (region.hasCeiling && joinX.slice1.projSliceBottom_wd < subview1) {
        subview1 = joinX.slice1.projSliceBottom_wd;
        projRay1 = LineSegment(Point(0, 0), tProjX0 * 9999.9);
      }

      zone = nextZone;
    }
    else if (X->kind == XWrapperKind::SPRITE) {
      SpriteX& spriteX = dynamic_cast<SpriteX&>(*X);

      double floorHeight = spriteX.vRect->zone->floorHeight;
      const Point& pt = spriteX.X->point_rel;

      LineSegment sprite(Point(pt.x, floorHeight + spriteX.vRect->height - cam.height),
        Point(pt.x, floorHeight + spriteX.vRect->height - cam.height + spriteX.vRect->size.y));

      Point projX0, projX1;
      spriteX.slice = computeSlice(rotProjPlane, sprite, subview0, subview1, projRay0, projRay1,
        projX0, projX1);
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
// Renderer::drawSkySlice
//===========================================
void Renderer::drawSkySlice(const RenderGraph& rg, const Camera& cam, const ScreenSlice& slice,
  int screenX_px) const {

  const Texture& skyTex = rg.textures.at("sky");
  Size tileSz_px(skyTex.image.rect().width(), skyTex.image.rect().height());

  int W_px = m_target.rect().width();
  int H_px = m_target.rect().height();

  double hPxAngle = cam.hFov / W_px;
  double vPxAngle = cam.vFov / H_px;

  double hAngle = normaliseAngle(cam.angle() - 0.5 * cam.hFov + hPxAngle * screenX_px);
  double s = hAngle / (2.0 * PI);
  assert(isBetween(s, 0.0, 1.0));

  int x = tileSz_px.x * s;

  double maxPitch = DEG_TO_RAD(20.0); // TODO
  double minVAngle = -0.5 * cam.vFov - maxPitch;
  double maxVAngle = 0.5 * cam.vFov + maxPitch;
  double vAngleRange = maxVAngle - minVAngle;
  double viewportBottomAngle = cam.vAngle - 0.5 * cam.vFov;

  for (int j = slice.viewportTop_px; j <= slice.sliceTop_px; ++j) {
    double vAngle = viewportBottomAngle + (H_px - j) * vPxAngle;
    double s = 1.0 - normaliseAngle(vAngle - minVAngle) / vAngleRange;
    assert(isBetween(s, 0.0, 1.0));

    assert(isBetween(screenX_px, 0, m_target.width() - 1) && isBetween(j, 0,
      m_target.height() - 1));

    QRgb* pixels = reinterpret_cast<QRgb*>(m_target.scanLine(j));
    pixels[screenX_px] = pixel(skyTex.image, x, s * tileSz_px.y);
  }
}

//===========================================
// worldPointToFloorTexel
//===========================================
inline static Point worldPointToFloorTexel(const Point& p, const Size& tileSz_wd_rp,
  const QRectF& frameRect_tx) {

  double nx = p.x * tileSz_wd_rp.x;
  double ny = p.y * tileSz_wd_rp.y;

  if (nx < 0 || std::isinf(nx) || std::isnan(nx)) {
    nx = 0;
  }
  if (ny < 0 || std::isinf(ny) || std::isnan(ny)) {
    ny = 0;
  }

  return Point(frameRect_tx.x() + (nx - static_cast<int>(nx)) * frameRect_tx.width(),
    frameRect_tx.y() + (ny - static_cast<int>(ny)) * frameRect_tx.height());
}

//===========================================
// Renderer::drawCeilingSlice
//===========================================
void Renderer::drawCeilingSlice(const RenderGraph& rg, const Camera& cam, const Intersection& X,
  const CRegion& region, double ceilingHeight, const ScreenSlice& slice, int screenX_px,
  double projX_wd) const {

  double screenH_px = rg.viewport.y * m_vWorldUnit_px; // TODO: Pre-compute this
  const Texture& ceilingTex = rg.textures.at(region.ceilingTexture);
  Size texSz_tx(ceilingTex.image.rect().width(), ceilingTex.image.rect().height());
  const QRectF& frameRect = region.ceilingTexRect;
  QRectF frameRect_tx(frameRect.x() * texSz_tx.x, frameRect.y() * texSz_tx.y,
    frameRect.width() * texSz_tx.x, frameRect.height() * texSz_tx.y);

  double hAngle = fastATan(projX_wd / cam.F);
  LineSegment ray(X.viewPoint, X.point_wld);

  Size tileSz_wd_rp(1.0 / ceilingTex.size_wd.x, 1.0 / ceilingTex.size_wd.y);

  double vWorldUnit_px_rp = 1.0 / m_vWorldUnit_px;
  double F_rp = 1.0 / cam.F;
  double rayLen = ray.length();
  double rayLen_rp = 1.0 / rayLen;
  double cosHAngle_rp = 1.0 / cos(hAngle);

  for (int j = slice.sliceTop_px; j >= slice.viewportTop_px; --j) {
    assert(isBetween(screenX_px, 0, m_target.width() - 1) && isBetween(j, 0,
      m_target.height() - 1));

    QRgb* pixels = reinterpret_cast<QRgb*>(m_target.scanLine(j));

    double projY_wd = (screenH_px * 0.5 - j) * vWorldUnit_px_rp;
    double vAngle = fastATan(projY_wd * F_rp) + cam.vAngle;
    double d_ = (ceilingHeight - cam.height) * fastTan_rp(vAngle);
    double d = d_ * cosHAngle_rp;
    if (!isBetween(d, 0, rayLen)) {
      d = rayLen;
    }
    double s = d * rayLen_rp;
    Point p(ray.A.x + (ray.B.x - ray.A.x) * s, ray.A.y + (ray.B.y - ray.A.y) * s);

    Point texel = worldPointToFloorTexel(p, tileSz_wd_rp, frameRect_tx);
    pixels[screenX_px] = applyShade(pixel(ceilingTex.image, texel.x, texel.y), d);
  }
}

//===========================================
// getFloorDecal
//
// x is set to the point inside decal space
//===========================================
static const CFloorDecal* getFloorDecal(const SpatialSystem& spatialSystem, const CRegion& region,
  const Point& pt, Point& x) {

  for (auto it = region.floorDecals.begin(); it != region.floorDecals.end(); ++it) {
    const CFloorDecal& decal = **it;

    const CHRect& hRect = getHRect(spatialSystem, decal);
    x = hRect.transform.inverse() * pt;

    if (isBetween(x.x, 0, hRect.size.x) && isBetween(x.y, 0, hRect.size.y)) {
      return &decal;
    }
  }

  return nullptr;
}

//===========================================
// Renderer::drawFloorSlice
//===========================================
void Renderer::drawFloorSlice(const RenderGraph& rg, const Camera& cam,
  const SpatialSystem& spatialSystem, const Intersection& X, const CRegion& region,
  double floorHeight, const ScreenSlice& slice, int screenX_px, double projX_wd) const {

  double screenH_px = rg.viewport.y * m_vWorldUnit_px;
  const Texture& floorTex = rg.textures.at(region.floorTexture);
  Size texSz_tx(floorTex.image.rect().width(), floorTex.image.rect().height());
  const QRectF& frameRect = region.floorTexRect;
  QRectF frameRect_tx(frameRect.x() * texSz_tx.x, frameRect.y() * texSz_tx.y,
    frameRect.width() * texSz_tx.x, frameRect.height() * texSz_tx.y);

  Size tileSz_wd_rp(1.0 / floorTex.size_wd.x, 1.0 / floorTex.size_wd.y);

  double hAngle = fastATan(projX_wd / cam.F);
  LineSegment ray(X.viewPoint, X.point_wld);

  double vWorldUnit_px_rp = 1.0 / m_vWorldUnit_px; // TODO: Store in Renderer instance?
  double F_rp = 1.0 / cam.F;
  double rayLen = ray.length();
  double rayLen_rp = 1.0 / rayLen;
  double cosHAngle_rp = 1.0 / cos(hAngle);

  for (int j = slice.sliceBottom_px; j <= slice.viewportBottom_px; ++j) {
    assert(isBetween(screenX_px, 0, m_target.width() - 1) && isBetween(j, 0,
      m_target.height() - 1));

    QRgb* pixels = reinterpret_cast<QRgb*>(m_target.scanLine(j));

    double projY_wd = (j - screenH_px * 0.5) * vWorldUnit_px_rp;
    double vAngle = fastATan(projY_wd * F_rp) - cam.vAngle;
    double d_ = (cam.height - floorHeight) * fastTan_rp(vAngle);
    double d = d_ * cosHAngle_rp;
    if (!isBetween(d, 0, rayLen)) {
      d = rayLen;
    }
    double s = d * rayLen_rp;

    Point p(ray.A.x + (ray.B.x - ray.A.x) * s, ray.A.y + (ray.B.y - ray.A.y) * s);

    Point decalPt;
    const CFloorDecal* decal = getFloorDecal(spatialSystem, region, p, decalPt);

    if (decal != nullptr) {
      const CHRect& hRect = getHRect(spatialSystem, *decal);

      // TODO: Use frameRect
      const Texture& decalTex = rg.textures.at(decal->texture);
      Size texSz_tx(decalTex.image.rect().width(), decalTex.image.rect().height());

      Point texel(texSz_tx.x * decalPt.x / hRect.size.x, texSz_tx.y * decalPt.y / hRect.size.y);
      pixels[screenX_px] = pixel(decalTex.image, texel.x, texel.y);
    }
    else {
      Point texel = worldPointToFloorTexel(p, tileSz_wd_rp, frameRect_tx);
      pixels[screenX_px] = applyShade(pixel(floorTex.image, texel.x, texel.y), d);
    }
  }
}

//===========================================
// Renderer::sampleSpriteTexture
//===========================================
QRect Renderer::sampleSpriteTexture(const Camera& cam, const QRect& rect, const SpriteX& X,
  const Size& size_wd, double y_wd) const {

  double H_tx = rect.height();
  double W_tx = rect.width();

  double hWorldUnit_tx = W_tx / size_wd.x;
  double texW_wd = W_tx / hWorldUnit_tx;

  double n = X.X->distanceAlongTarget / texW_wd;
  double x = (n - floor(n)) * texW_wd;

  double texBottom_tx = H_tx - ((cam.height + X.slice.sliceBottom_wd - y_wd) / size_wd.y) * H_tx;
  double texTop_tx = H_tx - ((cam.height + X.slice.sliceTop_wd - y_wd) / size_wd.y) * H_tx;

  int i = x * hWorldUnit_tx;

  return QRect(rect.x() + i, rect.y() + texTop_tx, 1, texBottom_tx - texTop_tx);
}

//===========================================
// Renderer::sampleWallTexture
//===========================================
void Renderer::sampleWallTexture(const Camera& cam, double screenX_px, double texAnchor_wd,
  double distanceAlongTarget, const Slice& slice, const Size& texSz_tx, const QRectF& frameRect,
  const Size& tileSz_wd, vector<QRect>& trgRects, vector<QRect>& srcRects) const {

  double projSliceH_wd = slice.projSliceTop_wd - slice.projSliceBottom_wd;
  double sliceH_wd = slice.sliceTop_wd - slice.sliceBottom_wd;
  double sliceToProjScale = projSliceH_wd / sliceH_wd;
  double projTexH_wd = tileSz_wd.y * sliceToProjScale;

  QRectF frameRect_tx(frameRect.x() * texSz_tx.x, frameRect.y() * texSz_tx.y,
    frameRect.width() * texSz_tx.x, frameRect.height() * texSz_tx.y);

  // World space
  double sliceBottom_wd = slice.sliceBottom_wd + cam.height;
  double sliceTop_wd = slice.sliceTop_wd + cam.height;

  // World space (not camera space)
  auto fnSliceToProjY = [&](double y) -> double {
    return slice.projSliceBottom_wd + (y - sliceBottom_wd) * sliceToProjScale;
  };

  double hWorldUnit_tx = frameRect_tx.width() / tileSz_wd.x;
  double vWorldUnit_tx = frameRect_tx.height() / tileSz_wd.y;

  double nx = distanceAlongTarget / tileSz_wd.x;
  double x = (nx - floor(nx)) * tileSz_wd.x;

  // Relative to tex anchor
  double texY0 = floor((sliceBottom_wd - texAnchor_wd) / tileSz_wd.y) * tileSz_wd.y;
  double texY1 = ceil((sliceTop_wd - texAnchor_wd) / tileSz_wd.y) * tileSz_wd.y;

  // World space
  double y0 = texY0 + texAnchor_wd;
  double y1 = texY1 + texAnchor_wd;

  double bottomOffset_wd = sliceBottom_wd - y0;
  double topOffset_wd = y1 - sliceTop_wd;

  // Ensure offsets are texel-aligned
  bottomOffset_wd = floor(bottomOffset_wd * vWorldUnit_tx) / vWorldUnit_tx;
  topOffset_wd = floor(topOffset_wd * vWorldUnit_tx) / vWorldUnit_tx;

  int j0 = floor(texY0 / tileSz_wd.y);
  int j1 = ceil(texY1 / tileSz_wd.y);

  for (int j = j0; j < j1; ++j) {
    double srcX = frameRect_tx.x() + x * hWorldUnit_tx;
    double srcY = frameRect_tx.y();
    double srcW = 1;
    double srcH = frameRect_tx.height();

    double y = texAnchor_wd + j * tileSz_wd.y;

    double trgX = screenX_px;
    double trgY = projToScreenY(fnSliceToProjY(y) + projTexH_wd);
    double trgW = 1;
    double trgH = projTexH_wd * m_vWorldUnit_px;

    // Note that screen y-axis is inverted
    if (j == j0) {
      srcH -= bottomOffset_wd * vWorldUnit_tx;
      trgH -= bottomOffset_wd * sliceToProjScale * m_vWorldUnit_px;
    }
    if (j + 1 == j1) {
      double srcDy = topOffset_wd * vWorldUnit_tx;
      srcY += srcDy;
      srcH -= srcDy;

      double trgDy = topOffset_wd * sliceToProjScale * m_vWorldUnit_px;
      trgY += trgDy;
      trgH -= trgDy;
    }

    srcRects.push_back(QRect(srcX, srcY, srcW, srcH));
    trgRects.push_back(QRect(trgX, trgY, trgW, ceil(trgH)));
  }
}

//===========================================
// Renderer::drawSlice
//===========================================
Renderer::ScreenSlice Renderer::drawSlice(const RenderGraph& rg, const Camera& cam,
  const Intersection& X, const Slice& slice, const string& texture, const QRectF& frameRect,
  double screenX_px, double targetH_wd) const {

  const Texture& wallTex = rg.textures.at(texture);

  const QRectF& rect = wallTex.image.rect();
  Size texSz_tx(rect.width(), rect.height());

  double screenSliceBottom_px = floor(m_viewport_px.y - slice.projSliceBottom_wd * m_vWorldUnit_px);
  double screenSliceTop_px = ceil(m_viewport_px.y - slice.projSliceTop_wd * m_vWorldUnit_px);

  if (screenSliceBottom_px - screenSliceTop_px > 0) {
    vector<QRect> srcRects;
    vector<QRect> trgRects;
    sampleWallTexture(cam, screenX_px, targetH_wd, X.distanceAlongTarget, slice, texSz_tx,
      frameRect, wallTex.size_wd, trgRects, srcRects);

    assert(srcRects.size() == trgRects.size());

    for (unsigned int i = 0; i < srcRects.size(); ++i) {
      drawImage(trgRects[i], wallTex.image, srcRects[i], X.distanceFromOrigin);
    }
  }

  double viewportBottom_px = ceil((rg.viewport.y - slice.viewportBottom_wd) * m_vWorldUnit_px);
  double viewportTop_px = floor((rg.viewport.y - slice.viewportTop_wd) * m_vWorldUnit_px);

  return ScreenSlice{
    static_cast<int>(clipNumber(screenSliceBottom_px, Range(0, m_target.height() - 1))),
    static_cast<int>(clipNumber(screenSliceTop_px, Range(0, m_target.height() - 1))),
    static_cast<int>(clipNumber(viewportBottom_px, Range(0, m_target.height() - 1))),
    static_cast<int>(clipNumber(viewportTop_px, Range(0, m_target.height() - 1)))};
}

//===========================================
// Renderer::drawSprite
//===========================================
void Renderer::drawSprite(const RenderGraph& rg, const Camera& camera, const SpriteX& X,
  double screenX_px) const {

  const CSprite& sprite = *X.sprite;
  const CVRect& vRect = *X.vRect;
  const Slice& slice = X.slice;

  const Texture& tex = rg.textures.at(sprite.texture);
  const QRectF& uv = sprite.getView(vRect, camera.pos());
  QRect r = tex.image.rect();
  QRect frame(r.width() * uv.x(), r.height() * uv.y(), r.width() * uv.width(),
    r.height() * uv.height());

  int screenSliceBottom_px = m_viewport_px.y - slice.projSliceBottom_wd * m_vWorldUnit_px;
  int screenSliceTop_px = m_viewport_px.y - slice.projSliceTop_wd * m_vWorldUnit_px;

  const CZone& zone = *vRect.zone;

  QRect srcRect = sampleSpriteTexture(camera, frame, X, vRect.size,
    zone.floorHeight + vRect.height);
  QRect trgRect(screenX_px, screenSliceTop_px, 1, screenSliceBottom_px - screenSliceTop_px);

  drawImage(trgRect, tex.image, srcRect, X.X->distanceFromOrigin);
}

//===========================================
// Renderer::drawWallDecal
//===========================================
void Renderer::drawWallDecal(const RenderGraph& rg, const Camera& cam,
  const SpatialSystem& spatialSystem, const CWallDecal& decal, const Intersection& X,
  const Slice& slice, const CZone& zone, int screenX_px) const {

  const CVRect& vRect = getVRect(spatialSystem, decal);

  const Texture& decalTex = rg.textures.at(decal.texture);

  int texX_px = decal.texRect.x() * decalTex.image.width();
  int texY_px = decal.texRect.y() * decalTex.image.height();
  int texW_px = decal.texRect.width() * decalTex.image.width();
  int texH_px = decal.texRect.height() * decalTex.image.height();

  double projSliceH_wd = slice.projSliceTop_wd - slice.projSliceBottom_wd;
  double sliceH_wd = slice.sliceTop_wd - slice.sliceBottom_wd;
  double sliceToProjScale = projSliceH_wd / sliceH_wd;

  // World space
  double sliceBottom_wd = slice.sliceBottom_wd + cam.height;

  // World space (not camera space)
  auto fnSliceToProjY = [&](double y) {
    return slice.projSliceBottom_wd + (y - sliceBottom_wd) * sliceToProjScale;
  };

  double floorH = zone.floorHeight;

  double y0 = floorH + vRect.pos.y;
  double y1 = floorH + vRect.pos.y + vRect.size.y;

  int y0_px = projToScreenY(fnSliceToProjY(y0));
  int y1_px = projToScreenY(fnSliceToProjY(y1));

  double x = X.distanceAlongTarget - vRect.pos.x;
  int i = texX_px + texW_px * x / vRect.size.x;

  QRect srcRect(i, texY_px, 1, texH_px);
  QRect trgRect(screenX_px, y1_px, 1, y0_px - y1_px);

  drawImage(trgRect, decalTex.image, srcRect, X.distanceFromOrigin);
}

//===========================================
// Renderer::drawColourOverlay
//===========================================
void Renderer::drawColourOverlay(const RenderGraph& rg, QPainter& painter,
  const CColourOverlay& overlay) const {

  double x = (overlay.pos.x / rg.viewport.x) * m_viewport_px.x;
  double y = (1.0 - overlay.pos.y / rg.viewport.y) * m_viewport_px.y;
  double w = (overlay.size.x / rg.viewport.x) * m_viewport_px.x;
  double h = (overlay.size.y / rg.viewport.y) * m_viewport_px.y;

  painter.setPen(Qt::NoPen);
  painter.setBrush(overlay.colour);
  painter.drawRect(x, y - h, w, h);
}

//===========================================
// Renderer::drawImageOverlay
//===========================================
void Renderer::drawImageOverlay(const RenderGraph& rg, QPainter& painter,
  const CImageOverlay& overlay) const {

  double x = (overlay.pos.x / rg.viewport.x) * m_viewport_px.x;
  double y = (1.0 - overlay.pos.y / rg.viewport.y) * m_viewport_px.y;
  double w = (overlay.size.x / rg.viewport.x) * m_viewport_px.x;
  double h = (overlay.size.y / rg.viewport.y) * m_viewport_px.y;

  const Texture& tex = rg.textures.at(overlay.texture);

  double sx = overlay.texRect.x() * tex.image.rect().width();
  double sy = overlay.texRect.y() * tex.image.rect().height();
  double sw = overlay.texRect.width() * tex.image.rect().width();
  double sh = overlay.texRect.height() * tex.image.rect().height();

  QRect srcRect(sx, sy, sw, sh);
  QRect trgRect(x, y - h, w, h);
  painter.drawImage(trgRect, tex.image, srcRect);
}

//===========================================
// Renderer::drawTextOverlay
//===========================================
void Renderer::drawTextOverlay(const RenderGraph& rg, QPainter& painter,
  const CTextOverlay& overlay) const {

  double x = (overlay.pos.x / rg.viewport.x) * m_viewport_px.x;
  double y = (1.0 - overlay.pos.y / rg.viewport.y) * m_viewport_px.y;
  double h = (overlay.height / rg.viewport.y) * m_viewport_px.y;

  QFont font;
  font.setPixelSize(h);

  painter.setFont(font);
  painter.setPen(overlay.colour);
  painter.drawText(x, y, overlay.text.c_str());
}

//===========================================
// Renderer::Renderer
//===========================================
Renderer::Renderer(EntityManager& entityManager, QImage& target)
  : m_entityManager(entityManager),
    m_target(target) {

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
// Renderer::getWallDecals
//===========================================
static vector<CWallDecal*> getWallDecals(const SpatialSystem& spatialSystem, const CBoundary& wall,
  double x) {

  vector<CWallDecal*> decals;
  decals.reserve(wall.decals.size());

  for (auto it = wall.decals.begin(); it != wall.decals.end(); ++it) {
    CWallDecal* decal = it->get();
    const CVRect& vRect = getVRect(spatialSystem, *decal);

    double x0 = vRect.pos.x;
    double x1 = vRect.pos.x + vRect.size.x;

    if (isBetween(x, x0, x1)) {
      decals.push_back(decal);
    }
  }

  std::sort(decals.begin(), decals.end(), [](const CWallDecal* a, const CWallDecal* b) {
    return a->zIndex < b->zIndex;
  });

  return decals;
}

//===========================================
// Renderer::renderScene
//===========================================
void Renderer::renderScene(const RenderGraph& rg, const Camera& cam) {
  const SpatialSystem& spatialSystem = m_entityManager
    .system<SpatialSystem>(ComponentKind::C_SPATIAL);
  const RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  QPainter painter;
  painter.begin(&m_target);

  // TODO: initialise this stuff outside of this function
  //
  m_viewport_px = Size(m_target.width(), m_target.height());

  m_hWorldUnit_px = m_viewport_px.x / rg.viewport.x;
  m_vWorldUnit_px = m_viewport_px.y / rg.viewport.y;

  QRect rect(QPoint(), QSize(m_viewport_px.x, m_viewport_px.y));
  painter.fillRect(rect, QBrush(QColor(255, 0, 0)));

  const int W = m_viewport_px.x;
  CastResult prev;

#pragma omp parallel for \
  num_threads(4) \
  private(prev)

  for (int screenX_px = 0; screenX_px < W; ++screenX_px) {
    double projX_wd = static_cast<double>(screenX_px - m_viewport_px.x / 2) / m_hWorldUnit_px;

    Vec2f ray(cam.F, projX_wd);

    CastResult result;
    castRay(rg, cam, spatialSystem, renderSystem, ray, result);

    // Hack to fill gaps where regions don't connect properly
    if (result.intersections.size() == 0) {
      result = std::move(prev);
    }

    for (auto it = result.intersections.rbegin(); it != result.intersections.rend(); ++it) {
      XWrapper& X = **it;

      if (X.kind == XWrapperKind::WALL) {
        const WallX& wallX = dynamic_cast<const WallX&>(X);

        const CZone& zone = *wallX.nearZone;
        const CRegion& region = dynamic_cast<const CRegion&>(renderSystem
          .getComponent(wallX.nearZone->entityId()));

        ScreenSlice slice = drawSlice(rg, cam, *wallX.X, wallX.slice, wallX.wall->texture,
          wallX.wall->texRect, screenX_px);

        vector<CWallDecal*> decals = getWallDecals(spatialSystem, *wallX.wall,
          wallX.X->distanceAlongTarget);

        for (auto decal : decals) {
          drawWallDecal(rg, cam, spatialSystem, *decal, *wallX.X, wallX.slice, zone, screenX_px);
        }

        drawFloorSlice(rg, cam, spatialSystem, *wallX.X, region, zone.floorHeight, slice,
          screenX_px, projX_wd);

        if (region.hasCeiling) {
          drawCeilingSlice(rg, cam, *wallX.X, region, zone.ceilingHeight, slice, screenX_px,
            projX_wd);
        }
        else {
          drawSkySlice(rg, cam, slice, screenX_px);
        }
      }
      else if (X.kind == XWrapperKind::JOIN) {
        const JoinX& joinX = dynamic_cast<const JoinX&>(X);

        ScreenSlice slice0 = drawSlice(rg, cam, *joinX.X, joinX.slice0, joinX.join->bottomTexture,
          joinX.join->bottomTexRect, screenX_px, joinX.farZone->floorHeight);

        const CZone& zone = *joinX.nearZone;
        const CRegion& region = dynamic_cast<const CRegion&>(renderSystem
          .getComponent(joinX.nearZone->entityId()));

        drawFloorSlice(rg, cam, spatialSystem, *joinX.X, region, zone.floorHeight, slice0,
          screenX_px, projX_wd);

        if (joinX.slice1.visible) {
          ScreenSlice slice1 = drawSlice(rg, cam, *joinX.X, joinX.slice1, joinX.join->topTexture,
            joinX.join->topTexRect, screenX_px, joinX.farZone->ceilingHeight);

          if (region.hasCeiling) {
            drawCeilingSlice(rg, cam, *joinX.X, region, zone.ceilingHeight, slice1, screenX_px,
              projX_wd);
          }
          else {
            drawSkySlice(rg, cam, slice1, screenX_px);
          }
        }

        vector<CWallDecal*> decals = getWallDecals(spatialSystem, *joinX.join,
          joinX.X->distanceAlongTarget);

        for (auto decal : decals) {
          Slice slice = joinX.slice0;

          if (joinX.slice1.visible) {
            slice.sliceTop_wd = joinX.slice1.sliceTop_wd;
            slice.projSliceTop_wd = joinX.slice1.projSliceTop_wd;
            slice.viewportTop_wd = joinX.slice1.viewportTop_wd;
          }

          drawWallDecal(rg, cam, spatialSystem, *decal, *joinX.X, slice, *joinX.nearZone,
            screenX_px);
        }
      }
      else if (X.kind == XWrapperKind::SPRITE) {
        const SpriteX& spriteX = dynamic_cast<const SpriteX&>(X);
        drawSprite(rg, cam, spriteX, screenX_px);
      }
    }

    prev = std::move(result);
  }

  for (auto it = rg.overlays.begin(); it != rg.overlays.end(); ++it) {
    const COverlay& overlay = **it;

    switch (overlay.kind) {
      case COverlayKind::IMAGE:
        drawImageOverlay(rg, painter, dynamic_cast<const CImageOverlay&>(overlay));
        break;
      case COverlayKind::TEXT:
        drawTextOverlay(rg, painter, dynamic_cast<const CTextOverlay&>(overlay));
        break;
      case COverlayKind::COLOUR:
        drawColourOverlay(rg, painter, dynamic_cast<const CColourOverlay&>(overlay));
        break;
    }
  }

  painter.end();
}
