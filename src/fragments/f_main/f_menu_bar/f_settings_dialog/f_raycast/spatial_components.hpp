#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SPATIAL_COMPONENTS_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SPATIAL_COMPONENTS_HPP__


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
  HARD_EDGE,
  SOFT_EDGE,
  V_RECT,
  H_RECT
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

class CVRect : public CSpatial {
  public:
    CVRect(entityId_t entityId, entityId_t parentId, const Size& size)
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

    virtual ~CVRect() override {}
};

typedef std::unique_ptr<CVRect> pCVRect_t;

struct CEdge : public CSpatial {
  CEdge(CSpatialKind kind, entityId_t entityId, entityId_t parentId)
    : CSpatial(kind, entityId, parentId) {}

  CEdge(const CEdge& cpy, entityId_t entityId, entityId_t parentId)
    : CSpatial(cpy.kind, entityId, parentId) {

    lseg = cpy.lseg;
  }

  LineSegment lseg;

  virtual ~CEdge() override {}
};

typedef std::unique_ptr<CEdge> pCEdge_t;

struct CHRect : public CSpatial {
  CHRect(entityId_t entityId, entityId_t parentId)
    : CSpatial(CSpatialKind::H_RECT, entityId, parentId) {}

  Size size;
  Matrix transform;

  virtual ~CHRect() override {}
};

typedef std::unique_ptr<CHRect> pCHRect_t;

typedef std::unique_ptr<CZone> pCZone_t;

struct CZone : public CSpatial {
  CZone(entityId_t entityId, entityId_t parentId)
    : CSpatial(CSpatialKind::ZONE, entityId, parentId) {}

  double floorHeight = 0;
  double ceilingHeight = 100;
  std::list<pCZone_t> children;
  std::list<CEdge*> edges;
  std::list<pCVRect_t> vRects;
  std::list<pCHRect_t> hRects;
  CZone* parent = nullptr;

  virtual ~CZone() override {}
};

void forEachConstZone(const CZone& zone, std::function<void(const CZone&)> fn);
void forEachZone(CZone& zone, std::function<void(CZone&)> fn);

struct CHardEdge : public CEdge {
  CHardEdge(entityId_t entityId, entityId_t parentId)
    : CEdge(CSpatialKind::HARD_EDGE, entityId, parentId) {}

  CZone* zone;
  std::list<pCVRect_t> vRects;

  double height() const {
    return zone->ceilingHeight - zone->floorHeight;
  }

  virtual ~CHardEdge() override {}
};

struct CSoftEdge : public CEdge {
  CSoftEdge(entityId_t entityId, entityId_t parentId, entityId_t joinId)
    : CEdge(CSpatialKind::SOFT_EDGE, entityId, parentId),
      joinId(joinId) {}

  CSoftEdge(const CSoftEdge& cpy, entityId_t entityId, entityId_t parentId, entityId_t joinId)
    : CEdge(cpy, entityId, parentId),
      joinId(joinId) {

    zoneA = cpy.zoneA;
    zoneB = cpy.zoneB;
  }

  entityId_t joinId = 0;

  CZone* zoneA = nullptr;
  CZone* zoneB = nullptr;

  virtual ~CSoftEdge() override {}
};


#endif
