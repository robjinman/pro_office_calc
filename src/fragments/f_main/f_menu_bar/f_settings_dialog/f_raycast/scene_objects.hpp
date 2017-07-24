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


enum class CSpatialKind {
  ZONE,
  WALL,
  JOINING_EDGE,
  V_RECT,
  FLOOR_DECAL
};

struct CSpatial : public Component {
  CSpatial(CSpatialKind kind, entityId_t entityId, entityId_t parentId)
    : Component(entityId, ComponentKind::C_SPATIAL),
      kind(kind),
      parentId(parentId) {}

  CSpatialKind kind;
  entityId_t parentId;

  virtual ~CSpatial() override {}
};

typedef std::unique_ptr<CSpatial> pCSpatial_t;

class CZone;

class VRect : public CSpatial {
  public:
    VRect(entityId_t entityId, entityId_t parentId, const Size& size)
      : CSpatial(CSpatialKind::V_RECT, entityId, parentId),
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

    virtual ~VRect() override {}
};

typedef std::unique_ptr<VRect> pVRect_t;

struct Edge : public CSpatial {
  Edge(CSpatialKind kind, entityId_t entityId, entityId_t parentId)
    : CSpatial(kind, entityId, parentId) {}

  Edge(const Edge& cpy, entityId_t entityId, entityId_t parentId)
    : CSpatial(cpy.kind, entityId, parentId) {

    lseg = cpy.lseg;
  }

  LineSegment lseg;

  virtual ~Edge() {}
};

typedef std::unique_ptr<Edge> pEdge_t;

struct FloorDecal : public CSpatial {
  FloorDecal(entityId_t entityId, entityId_t parentId)
    : CSpatial(CSpatialKind::FLOOR_DECAL, entityId, parentId) {}

  Size size;
  Matrix transform;

  virtual ~FloorDecal() override {}
};

typedef std::unique_ptr<FloorDecal> pFloorDecal_t;

typedef std::unique_ptr<CZone> pCZone_t;

struct CZone : public CSpatial {
  CZone(entityId_t entityId, entityId_t parentId)
    : CSpatial(CSpatialKind::ZONE, entityId, parentId) {}

  double floorHeight = 0;
  double ceilingHeight = 100;
  std::list<pCZone_t> children;
  std::list<Edge*> edges;
  std::list<pVRect_t> sprites;
  std::list<pFloorDecal_t> floorDecals;
  CZone* parent = nullptr;

  virtual ~CZone() override {}
};

void forEachConstZone(const CZone& zone, std::function<void(const CZone&)> fn);
void forEachZone(CZone& zone, std::function<void(CZone&)> fn);

struct Wall : public Edge {
  Wall(entityId_t entityId, entityId_t parentId)
    : Edge(CSpatialKind::WALL, entityId, parentId) {}

  CZone* zone;
  std::list<pVRect_t> decals;

  double height() const {
    return zone->ceilingHeight - zone->floorHeight;
  }

  virtual ~Wall() {}
};

struct JoiningEdge : public Edge {
  JoiningEdge(entityId_t entityId, entityId_t parentId, entityId_t joinId)
    : Edge(CSpatialKind::JOINING_EDGE, entityId, parentId),
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
