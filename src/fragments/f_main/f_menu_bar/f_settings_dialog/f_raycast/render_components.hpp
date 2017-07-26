#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_COMPONENTS_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_COMPONENTS_HPP__


#include <string>
#include <list>
#include <memory>
#include <QImage>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"
#include "exception.hpp"


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

struct CRender : public Component {
  CRender(CRenderKind kind, entityId_t entityId, entityId_t parentId)
    : Component(entityId, ComponentKind::C_RENDER),
      kind(kind),
      parentId(parentId) {}

  CRenderKind kind;
  entityId_t parentId;

  virtual ~CRender() override {}
};

typedef std::unique_ptr<CRender> pCRender_t;

class COverlay : public CRender {
  public:
    COverlay(entityId_t entityId, const std::string& texture)
      : CRender(CRenderKind::OVERLAY, entityId, -1) {}

    // TODO
};

class CRegion;

class CSprite : public CRender {
  public:
    CSprite(entityId_t entityId, entityId_t parentId, const std::string& texture)
      : CRender(CRenderKind::SPRITE, entityId, parentId),
        texture(texture) {}

    const QRectF& getView(const VRect& vRect, const Point& camPos) const {
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

    CRegion* region;
    std::string texture;
    std::vector<QRectF> texViews;

    virtual ~CSprite() override {}
};

typedef std::unique_ptr<CSprite> pCSprite_t;

struct CBoundary : public CRender {
  CBoundary(CRenderKind kind, entityId_t entityId, entityId_t parentId)
    : CRender(kind, entityId, parentId) {}

  CBoundary(const CBoundary& cpy, entityId_t entityId, entityId_t parentId)
    : CRender(cpy.kind, entityId, parentId) {}

  virtual ~CBoundary() {}
};

typedef std::unique_ptr<CBoundary> pCBoundary_t;

struct CFloorDecal : public CRender {
  CFloorDecal(entityId_t entityId, entityId_t parentId)
    : CRender(CRenderKind::FLOOR_DECAL, entityId, parentId) {}

  std::string texture;

  virtual ~CFloorDecal() override {}
};

typedef std::unique_ptr<CFloorDecal> pCFloorDecal_t;

class CRegion;
typedef std::unique_ptr<CRegion> pCRegion_t;

struct CRegion : public CRender {
  CRegion(entityId_t entityId, entityId_t parentId)
    : CRender(CRenderKind::REGION, entityId, parentId) {}

  bool hasCeiling = true;
  std::string floorTexture;
  std::string ceilingTexture;
  std::list<pCRegion_t> children;
  std::list<CBoundary*> boundaries;
  std::list<pCSprite_t> sprites;
  std::list<pCFloorDecal_t> floorDecals;

  virtual ~CRegion() override {}
};

void forEachConstCRegion(const CRegion& region, std::function<void(const CRegion&)> fn);
void forEachCRegion(CRegion& region, std::function<void(CRegion&)> fn);

struct CWallDecal : public CRender {
  CWallDecal(entityId_t entityId, entityId_t parentId)
    : CRender(CRenderKind::WALL_DECAL, entityId, parentId) {}

  std::string texture;

  virtual ~CWallDecal() {}
};

typedef std::unique_ptr<CWallDecal> pCWallDecal_t;

struct CWall : public CBoundary {
  CWall(entityId_t entityId, entityId_t parentId)
    : CBoundary(CRenderKind::WALL, entityId, parentId) {}

  std::string texture;
  CRegion* region;
  std::list<pCWallDecal_t> decals;

  virtual ~CWall() {}
};

struct CJoin : public CBoundary {
  CJoin(entityId_t entityId, entityId_t parentId, entityId_t joinId)
    : CBoundary(CRenderKind::JOIN, entityId, parentId),
      joinId(joinId) {}

  CJoin(const CJoin& cpy, entityId_t entityId, entityId_t parentId, entityId_t joinId)
    : CBoundary(cpy, entityId, parentId),
      joinId(joinId) {

    topTexture = cpy.topTexture;
    bottomTexture = cpy.bottomTexture;
    regionA = cpy.regionA;
    regionB = cpy.regionB;
  }

  void mergeIn(const CJoin& other) {
    if (other.topTexture != "default") {
      topTexture = other.topTexture;
    }
    if (other.bottomTexture != "default") {
      bottomTexture = other.bottomTexture;
    }
  }

  entityId_t joinId = 0;

  std::string topTexture = "default";
  std::string bottomTexture = "default";

  CRegion* regionA = nullptr;
  CRegion* regionB = nullptr;

  virtual ~CJoin() {}
};


#endif
