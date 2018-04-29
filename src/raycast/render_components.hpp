#ifndef __PROCALC_RAYCAST_RENDER_COMPONENTS_HPP__
#define __PROCALC_RAYCAST_RENDER_COMPONENTS_HPP__


#include <string>
#include <list>
#include <memory>
#include <QImage>
#include <QColor>
#include "raycast/geometry.hpp"
#include "raycast/component.hpp"
#include "raycast/spatial_components.hpp"
#include "exception.hpp"
#include "utils.hpp"


struct Texture {
  QImage image;
  Size size_wd;
};

enum class CRenderKind {
  REGION,
  WALL,
  JOIN,
  SPRITE,
  FLOOR_DECAL,
  WALL_DECAL,
  OVERLAY
};

class CRender : public Component {
  public:
    CRender(CRenderKind kind, entityId_t entityId, entityId_t parentId)
      : Component(entityId, ComponentKind::C_RENDER),
        kind(kind),
        parentId(parentId) {}

    CRenderKind kind;
    entityId_t parentId;
};

typedef std::unique_ptr<CRender> pCRender_t;

enum class COverlayKind {
  IMAGE,
  TEXT,
  COLOUR
};

class COverlay : public CRender {
  public:
    COverlay(COverlayKind kind, entityId_t entityId, const Point& pos, int zIndex)
      : CRender(CRenderKind::OVERLAY, entityId, -1),
        kind(kind),
        pos(pos),
        zIndex(zIndex) {}

    COverlayKind kind;
    Point pos;
    int zIndex;
};

typedef std::unique_ptr<COverlay> pCOverlay_t;

inline bool operator<(const pCOverlay_t& lhs, const pCOverlay_t& rhs) {
  return lhs->zIndex < rhs->zIndex;
}

class CColourOverlay : public COverlay {
  public:
    CColourOverlay(entityId_t entityId, const QColor& colour, const Point& pos, const Size& size,
      int zIndex = 0)
      : COverlay(COverlayKind::COLOUR, entityId, pos, zIndex),
        colour(colour),
        size(size) {}

    QColor colour;
    Size size;
};

typedef std::unique_ptr<COverlay> pCOverlay_t;

class CImageOverlay : public COverlay {
  public:
    CImageOverlay(entityId_t entityId, const std::string& texture, const Point& pos,
      const Size& size, int zIndex = 0)
      : COverlay(COverlayKind::IMAGE, entityId, pos, zIndex),
        texture(texture),
        size(size) {}

    std::string texture;
    QRectF texRect = QRectF(0, 0, 1, 1);
    Size size;
};

typedef std::unique_ptr<CImageOverlay> pCImageOverlay_t;

class CTextOverlay : public COverlay {
  public:
    CTextOverlay(entityId_t entityId, std::string text, const Point& pos, double height,
      const QColor& colour, int zIndex)
      : COverlay(COverlayKind::TEXT, entityId, pos, zIndex),
        text(text),
        height(height),
        colour(colour) {}

    std::string text;
    double height;
    QColor colour;
};

typedef std::unique_ptr<CTextOverlay> pCTextOverlay_t;

class CRegion;

class CSprite : public CRender {
  public:
    CSprite(entityId_t entityId, entityId_t parentId, const std::string& texture)
      : CRender(CRenderKind::SPRITE, entityId, parentId),
        texture(texture) {}

    const QRectF& getView(const CVRect& vRect, const Point& camPos) const {
      if (texViews.empty()) {
        EXCEPTION("Cannot get view; texViews is empty");
      }

      Vec2f v = vRect.pos - camPos;
      double a = PI - atan2(v.y, v.x) + vRect.angle;
      int n = texViews.size();
      double da = 2.0 * PI / n;
      int idx = static_cast<int>(round(normaliseAngle(a) / da)) % n;
      return texViews[idx];
    }

    std::string texture;
    std::vector<QRectF> texViews;
};

typedef std::unique_ptr<CSprite> pCSprite_t;

class CWallDecal : public CRender {
  public:
    CWallDecal(entityId_t entityId, entityId_t parentId)
      : CRender(CRenderKind::WALL_DECAL, entityId, parentId) {}

    std::string texture;
    QRectF texRect = QRectF(0, 0, 1, 1);
    int zIndex = 0;
};

typedef std::unique_ptr<CWallDecal> pCWallDecal_t;

class CBoundary : public CRender {
  public:
    CBoundary(CRenderKind kind, entityId_t entityId, entityId_t parentId)
      : CRender(kind, entityId, parentId) {}

    std::list<pCWallDecal_t> decals;
};

typedef std::unique_ptr<CBoundary> pCBoundary_t;

class CFloorDecal : public CRender {
  public:
    CFloorDecal(entityId_t entityId, entityId_t parentId)
      : CRender(CRenderKind::FLOOR_DECAL, entityId, parentId) {}

    std::string texture;
};

typedef std::unique_ptr<CFloorDecal> pCFloorDecal_t;

class CRegion;
typedef std::unique_ptr<CRegion> pCRegion_t;

class CRegion : public CRender {
  public:
    CRegion(entityId_t entityId, entityId_t parentId)
      : CRender(CRenderKind::REGION, entityId, parentId) {}

    bool hasCeiling = true; // TODO: Remove
    std::string floorTexture;
    QRectF floorTexRect = QRectF(0, 0, 1, 1);
    std::string ceilingTexture;
    QRectF ceilingTexRect = QRectF(0, 0, 1, 1);
    std::list<pCRegion_t> children;
    std::list<CBoundary*> boundaries;
    std::list<pCSprite_t> sprites;
    std::list<pCFloorDecal_t> floorDecals;
};

void forEachConstCRegion(const CRegion& region, std::function<void(const CRegion&)> fn);
void forEachCRegion(CRegion& region, std::function<void(CRegion&)> fn);

class CWall : public CBoundary {
  public:
    CWall(entityId_t entityId, entityId_t parentId)
      : CBoundary(CRenderKind::WALL, entityId, parentId) {}

    std::string texture;
    QRectF texRect = QRectF(0, 0, 1, 1);
    CRegion* region;
};

class CJoin : public CBoundary {
  public:
    CJoin(entityId_t entityId, entityId_t parentId, entityId_t joinId)
      : CBoundary(CRenderKind::JOIN, entityId, parentId),
        joinId(joinId) {}

    void mergeIn(const CJoin& other) {
      if (topTexture == "default") {
        topTexture = other.topTexture;
      }
      if (bottomTexture == "default") {
        bottomTexture = other.bottomTexture;
      }
    }

    entityId_t joinId = 0;

    std::string topTexture = "default";
    std::string bottomTexture = "default";

    QRectF topTexRect = QRectF(0, 0, 1, 1);
    QRectF bottomTexRect = QRectF(0, 0, 1, 1);

    CRegion* regionA = nullptr;
    CRegion* regionB = nullptr;
};


#endif
