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


enum class CRenderSpatialKind {
  ZONE,
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

class CZone;

class Sprite : public CRenderSpatial {
  public:
    Sprite(entityId_t entityId, entityId_t parentId, const Size& size)
      : CRenderSpatial(CRenderSpatialKind::SPRITE, entityId, parentId),
        size(size) {}

    void setTransform(const Matrix& m) {
      pos.x = m.tx();
      pos.y = m.ty();
      angle = m.a();
    }

    CZone* zone;
    Vec2f pos;
    double angle;
    Size size;

    virtual ~Sprite() override {}
};

typedef std::unique_ptr<Sprite> pSprite_t;

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

  Size size;
  Matrix transform;

  virtual ~FloorDecal() override {}
};

typedef std::unique_ptr<FloorDecal> pFloorDecal_t;

typedef std::unique_ptr<CZone> pCZone_t;

struct CZone : public CRenderSpatial {
  CZone(entityId_t entityId, entityId_t parentId)
    : CRenderSpatial(CRenderSpatialKind::ZONE, entityId, parentId) {}

  double floorHeight = 0;
  double ceilingHeight = 100;
  std::list<pCZone_t> children;
  std::list<Edge*> edges;
  std::list<pSprite_t> sprites;
  std::list<pFloorDecal_t> floorDecals;
  CZone* parent = nullptr;

  virtual ~CZone() override {}
};

void forEachConstZone(const CZone& zone, std::function<void(const CZone&)> fn);
void forEachZone(CZone& zone, std::function<void(CZone&)> fn);

struct WallDecal : public CRenderSpatial {
  WallDecal(entityId_t entityId, entityId_t parentId)
    : CRenderSpatial(CRenderSpatialKind::WALL_DECAL, entityId, parentId) {}

  Size size;
  Point pos;

  virtual ~WallDecal() {}
};

typedef std::unique_ptr<WallDecal> pWallDecal_t;

struct Wall : public Edge {
  Wall(entityId_t entityId, entityId_t parentId)
    : Edge(CRenderSpatialKind::WALL, entityId, parentId) {}

  CZone* zone;
  std::list<pWallDecal_t> decals;

  double height() const {
    return zone->ceilingHeight - zone->floorHeight;
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

    zoneA = cpy.zoneA;
    zoneB = cpy.zoneB;
  }

  entityId_t joinId = 0;

  CZone* zoneA = nullptr;
  CZone* zoneB = nullptr;

  virtual ~JoiningEdge() {}
};


#endif
