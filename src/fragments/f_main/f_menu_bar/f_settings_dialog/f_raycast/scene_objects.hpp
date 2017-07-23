#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_OBJECTS_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_OBJECTS_HPP__


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

enum class CRenderSpatialKind {
  REGION,
  WALL,
  JOINING_EDGE,
  SPRITE,
  FLOOR_DECAL,
  WALL_DECAL
};

struct CRenderSpatial : public Component {
  CRenderSpatial(CRenderSpatialKind kind, entityId_t entityId, entityId_t parentId)
    : Component(entityId, ComponentKind::C_RENDER_SPATIAL),
      kind(kind),
      parentId(parentId) {}

  CRenderSpatialKind kind;
  entityId_t parentId;

  virtual ~CRenderSpatial() override {}
};

typedef std::unique_ptr<CRenderSpatial> pCRenderSpatial_t;

class Region;

class Sprite : public CRenderSpatial {
  public:
    Sprite(entityId_t entityId, entityId_t parentId, const Size& size, const std::string& texture)
      : CRenderSpatial(CRenderSpatialKind::SPRITE, entityId, parentId),
        texture(texture),
        size(size) {}

    void setTransform(const Matrix& m) {
      pos.x = m.tx();
      pos.y = m.ty();
      angle = m.a();
    }

    const QRectF& textureRegion(const Point& camPos) const {
      Vec2f v = pos - camPos;
      return animations.at("idle").currentFrame().part(PI - atan2(v.y, v.x) + angle);
    }

    Region* region;
    std::string texture;
    Vec2f pos;
    double angle;
    Size size;
    std::map<std::string, Animation> animations;

    void playAnimation(const std::string& name);

    virtual ~Sprite() override {}
};

typedef std::unique_ptr<Sprite> pSprite_t;

struct Ammo : public Sprite {
  Ammo(entityId_t entityId, entityId_t parentId)
    : Sprite(entityId, parentId, Size(30, 15), "ammo") {

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

  virtual ~Ammo() override {}
};

struct BadGuy : public Sprite {
  BadGuy(entityId_t entityId, entityId_t parentId)
    : Sprite(entityId, parentId, Size(70, 70), "bad_guy") {

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

  virtual ~BadGuy() override {}
};

struct Edge : public CRenderSpatial {
  Edge(CRenderSpatialKind kind, entityId_t entityId, entityId_t parentId)
    : CRenderSpatial(kind, entityId, parentId) {}

  Edge(const Edge& cpy, entityId_t entityId, entityId_t parentId)
    : CRenderSpatial(cpy.kind, entityId, parentId) {

    lseg = cpy.lseg;
  }

  LineSegment lseg;

  virtual ~Edge() {}
};

typedef std::unique_ptr<Edge> pEdge_t;

struct FloorDecal : public CRenderSpatial {
  FloorDecal(entityId_t entityId, entityId_t parentId)
    : CRenderSpatial(CRenderSpatialKind::FLOOR_DECAL, entityId, parentId) {}

  std::string texture;
  Size size;
  Matrix transform;

  virtual ~FloorDecal() override {}
};

typedef std::unique_ptr<FloorDecal> pFloorDecal_t;

class Region;
typedef std::unique_ptr<Region> pRegion_t;

struct Region : public CRenderSpatial {
  Region(entityId_t entityId, entityId_t parentId)
    : CRenderSpatial(CRenderSpatialKind::REGION, entityId, parentId) {}

  bool hasCeiling = true;
  double floorHeight = 0;
  double ceilingHeight = 100;
  std::string floorTexture;
  std::string ceilingTexture;
  std::list<pRegion_t> children;
  std::list<Edge*> edges;
  std::list<pSprite_t> sprites;
  std::list<pFloorDecal_t> floorDecals;
  Region* parent = nullptr;

  virtual ~Region() override {}
};

void forEachConstRegion(const Region& region, std::function<void(const Region&)> fn);
void forEachRegion(Region& region, std::function<void(Region&)> fn);

struct WallDecal : public CRenderSpatial {
  WallDecal(entityId_t entityId, entityId_t parentId)
    : CRenderSpatial(CRenderSpatialKind::WALL_DECAL, entityId, parentId) {}

  std::string texture;
  Size size;
  Point pos;

  virtual ~WallDecal() {}
};

typedef std::unique_ptr<WallDecal> pWallDecal_t;

struct Wall : public Edge {
  Wall(entityId_t entityId, entityId_t parentId)
    : Edge(CRenderSpatialKind::WALL, entityId, parentId) {}

  std::string texture;
  Region* region;
  std::list<pWallDecal_t> decals;

  double height() const {
    return region->ceilingHeight - region->floorHeight;
  }

  virtual ~Wall() {}
};

struct JoiningEdge : public Edge {
  JoiningEdge(entityId_t entityId, entityId_t parentId, entityId_t joinId)
    : Edge(CRenderSpatialKind::JOINING_EDGE, entityId, parentId),
      joinId(joinId) {}

  JoiningEdge(const JoiningEdge& cpy, entityId_t entityId, entityId_t parentId, entityId_t joinId)
    : Edge(cpy, entityId, parentId),
      joinId(joinId) {

    topTexture = cpy.topTexture;
    bottomTexture = cpy.bottomTexture;
    regionA = cpy.regionA;
    regionB = cpy.regionB;
  }

  void mergeIn(const JoiningEdge& other) {
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

  Region* regionA = nullptr;
  Region* regionB = nullptr;

  virtual ~JoiningEdge() {}
};


#endif
