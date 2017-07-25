#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_COMPONENTS_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_COMPONENTS_HPP__


#include <string>
#include <list>
#include <map>
#include <memory>
#include <QImage>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"


struct AnimationFrame {
  std::array<QRectF, 8> parts;

  const QRectF& part(double angle) const {
    double da = PI * 0.25;
    return parts[static_cast<int>(round(normaliseAngle(angle) / da)) % 8];
  }
};

class Animation {
  public:
    int fps = 0;
    std::vector<AnimationFrame> frames;

    void update();
    const AnimationFrame& currentFrame() const {
      return frames[m_currentFrameIdx];
    }

  private:
    double m_elapsed = 0.0;
    int m_currentFrameIdx = 0;
};

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
  WALL_DECAL
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

class CRegion;

class CSprite : public CRender {
  public:
    CSprite(entityId_t entityId, entityId_t parentId, const std::string& texture)
      : CRender(CRenderKind::SPRITE, entityId, parentId),
        texture(texture) {}

    const QRectF& textureRegion(const VRect& sprite, const Point& camPos) const {
      Vec2f v = sprite.pos - camPos;
      return animations.at("idle").currentFrame().part(PI - atan2(v.y, v.x) + sprite.angle);
    }

    CRegion* region;
    std::string texture;
    std::map<std::string, Animation> animations;

    void playAnimation(const std::string& name);

    virtual ~CSprite() override {}
};

typedef std::unique_ptr<CSprite> pCSprite_t;

struct CAmmo : public CSprite {
  CAmmo(entityId_t entityId, entityId_t parentId)
    : CSprite(entityId, parentId, "ammo") {

    Animation anim;
    anim.fps = 0;

    anim.frames.push_back(AnimationFrame{
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1)
    });

    animations["idle"] = anim;
  }

  virtual ~CAmmo() override {}
};

struct CBadGuy : public CSprite {
  CBadGuy(entityId_t entityId, entityId_t parentId)
    : CSprite(entityId, parentId, "bad_guy") {

    Animation anim;
    anim.fps = 0;

    anim.frames.push_back(AnimationFrame{
      QRectF(0, 0, 0.125, 1),
      QRectF(0.125, 0, 0.125, 1),
      QRectF(0.25, 0, 0.125, 1),
      QRectF(0.375, 0, 0.125, 1),
      QRectF(0.5, 0, 0.125, 1),
      QRectF(0.625, 0, 0.125, 1),
      QRectF(0.750, 0, 0.125, 1),
      QRectF(0.875, 0, 0.125, 1)
    });

    animations["idle"] = anim;
  }

  virtual ~CBadGuy() override {}
};

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
