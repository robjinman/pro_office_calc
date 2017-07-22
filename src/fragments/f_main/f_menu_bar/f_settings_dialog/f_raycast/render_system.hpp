#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_SYSTEM_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_SYSTEM_HPP_


#include <memory>
#include <vector>
#include <set>
#include <map>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"



/*
struct WallDecalNode {
  entityId_t entityId;
};

struct BoundaryNode {
  entityId_t entity;
  BoundaryNodeKind kind;
  std::list<WallDecalNode> decals;
};

struct SpriteNode {
  entityId_t entity;
};

struct FloorDecalNode {
  entityId_t entity;
};

struct RegionNode {
  entityId_t entity;
  std::list<RegionNode> regions;
  std::list<BoundaryNode> boundary;
  std::list<SpriteNode> sprites;
  std::list<FloorDecalNode> floorDecals;
};








struct CRender : public Component {
  CRender(entityId_t entityId)
    : Component(entityId, ComponentKind::C_RENDER) {}

  pRenderPrimitive_t primitive;

  virtual ~CRender() override {}
};

enum class RPrimKind {
  REGION,
  JOIN,
  WALL,
  SPRITE,
  FLOOR_DECAL,
  WALL_DECAL
};

struct RenderPrimitive {
  RenderPrimitive(RPrimKind kind)
    : kind(kind) {}

  RPrimKind kind;

  virtual ~RenderPrimitive() {}
};

typedef std::unique_ptr<RenderPrimitive> pRenderPrimitive_t;

struct Boundary : public RenderPrimitive {
  Boundary(RPrimKind kind, Edge& edge)
    : CRenderPrimitive(kind),
      edge(edge) {}

  Edge& edge;

  virtual ~Boundary() override {}
};

typedef std::unique_ptr<Boundary> pBoundary_t;

struct FloorDecal : public RenderPrimitive {
  FloorDecal(HRect& hRect)
    : RenderPrimitive(RPrimKind::FLOOR_DECAL),
      hRect(hRect) {}

  HRect& hRect;

  virtual ~FloorDecal() override {}
};

typedef std::unique_ptr<FloorDecal> pFloorDecal_t;

struct WallDecal : public RenderPrimitive {
  WallDecal(VRect& vRect)
    : RenderPrimitive(RPrimKind::WALL_DECAL),
      vRect(vRect) {}

  VRect& vRect;

  virtual ~WallDecal() override {}
};

typedef std::unique_ptr<WallDecal> pWallDecal_t;

struct Sprite : public RenderPrimitive {
  WallDecal(VRect& vRect)
    : RenderPrimitive(RPrimKind::WALL_DECAL),
      vRect(vRect) {}

  VRect& vRect;

  virtual ~WallDecal() override {}
};

typedef std::unique_ptr<WallDecal> pWallDecal_t;

struct HardEdge : public Edge {
  HardEdge()
    : Edge(SPrimKind::HARD_EDGE) {}

  Zone* zone;
  std::vector<VRect*> rects;

  double height() const {
    return zone->ceilingHeight - zone->floorHeight;
  }

  virtual ~HardEdge() override {}
};

typedef std::unique_ptr<HardEdge> pHardEdge_t;

struct SoftEdge : public Edge {
  SoftEdge()
    : Edge(SPrimKind::SOFT_EDGE) {}

  Zone* zoneA = nullptr;
  Zone* zoneB = nullptr;

  virtual ~SoftEdge() override {}
};

typedef std::unique_ptr<SoftEdge> pSoftEdge_t;

struct Zone : public SpatialPrimitive {
  Zone()
    : SpatialPrimitive(SPrimKind::ZONE) {}

  double floorHeight = 0;
  double ceilingHeight = 100;
  std::vector<Edge*> edges;
  std::vector<HRect*> hRects;
  std::vector<VRect*> vRects;

  Zone* parent = nullptr;
  std::vector<Zone*> children;
};

typedef std::unique_ptr<Zone> pZone_t;

class Event;

struct CSpatial : public Component {
  CSpatial(entityId_t entityId, entityId_t parent)
    : Component(entityId, ComponentKind::C_SPATIAL),
      parent(parent) {}

  entityId_t parent;
  pSpatialPrimitive_t primitive;

  virtual ~CSpatial() override {}
};

typedef std::unique_ptr<CSpatial> pCSpatial_t;

class SpatialSystem : public System {
  public:
    virtual void update() override;
    virtual void handleEvent(const Event& event) override;

    virtual void addComponent(pComponent_t component) override;
    virtual void removeComponent(entityId_t id) override;

    virtual ~SpatialSystem() override {}

  private:
    pZone_t m_rootZone;
    std::map<entityId_t, pCSpatial_t> m_components;
    std::map<entityId_t, std::set<entityId_t>> m_entityChildren;
};*/


#endif
