#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SPATIAL_SYSTEM_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_SPATIAL_SYSTEM_HPP_


#include <memory>
#include <list>
#include <set>
#include <map>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"


enum class CSpatialKind {
  ZONE,
  SOFT_EDGE,
  HARD_EDGE,
  V_RECT,
  H_RECT
};

struct CSpatial : public Component {
  CSpatial(CSpatialKind kind, entityId_t entityId, entityId_t parent)
    : Component(entityId, ComponentKind::C_SPATIAL),
      kind(kind),
      parent(parent) {}

  CSpatialKind kind;
  entityId_t parent;

  virtual ~CSpatial() override {}
};

typedef std::unique_ptr<CSpatial> pCSpatial_t;

struct CEdge : public CSpatial {
  CEdge(CSpatialKind kind, entityId_t entityId, entityId_t parent)
    : CSpatial(kind, entityId, parent) {} // TODO: assert one of SOFT_EDGE, HARD_EDGE

  LineSegment lseg;

  virtual ~CEdge() override {}
};

typedef std::unique_ptr<CEdge> pCEdge_t;

struct CHRect : public CSpatial {
  CHRect(entityId_t entityId, entityId_t parent)
    : CSpatial(CSpatialKind::H_RECT, entityId, parent) {}

  Size size;
  Matrix transform;

  virtual ~CHRect() override {}
};

typedef std::unique_ptr<CHRect> pCHRect_t;

struct CVRect : public CSpatial {
  CVRect(entityId_t entityId, entityId_t parent)
    : CSpatial(CSpatialKind::V_RECT, entityId, parent) {}

  Size size;
  Point pos;

  virtual ~CVRect() override {}
};

typedef std::unique_ptr<CVRect> pCVRect_t;

class CZone;
typedef std::unique_ptr<CZone> pCZone_t;

struct CZone : public CSpatial {
  CZone(entityId_t entityId, entityId_t parent)
    : CSpatial(CSpatialKind::ZONE, entityId, parent) {}

  double floorHeight = 0;
  double ceilingHeight = 100;
  std::list<pCEdge_t> edges;
  std::list<pCHRect_t> hRects;
  std::list<pCVRect_t> vRects;

  CZone* parent = nullptr;
  std::list<pCZone_t> children;

  virtual ~CZone() override {}
};

struct CHardEdge : public CEdge {
  CHardEdge(entityId_t entityId, entityId_t parent)
    : CEdge(CSpatialKind::HARD_EDGE, entityId, parent) {}

  CZone* zone;
  std::list<pCVRect_t> vRects;

  double height() const {
    return zone->ceilingHeight - zone->floorHeight;
  }

  virtual ~CHardEdge() override {}
};

typedef std::unique_ptr<CHardEdge> pCHardEdge_t;

struct CSoftEdge : public CEdge {
  CSoftEdge(entityId_t entityId, entityId_t parent)
    : CEdge(CSpatialKind::SOFT_EDGE, entityId, parent) {}

  CZone* zoneA = nullptr;
  CZone* zoneB = nullptr;

  virtual ~CSoftEdge() override {}
};

typedef std::unique_ptr<CSoftEdge> pCSoftEdge_t;

class Event;

class SpatialSystem : public System {
  public:
    virtual void update() override;
    virtual void handleEvent(const Event& event) override;

    virtual void addComponent(pComponent_t component) override;
    virtual void removeEntity(entityId_t id) override;

    virtual ~SpatialSystem() override {}

  private:
    pCZone_t m_rootZone;
    std::map<entityId_t, CSpatial*> m_components;
    std::map<entityId_t, std::set<entityId_t>> m_entityChildren;

    bool isRoot(const CSpatial& c) const;
    void removeEntity_r(entityId_t id);
};


#endif
