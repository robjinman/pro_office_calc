#ifndef __PROCALC_RAYCAST_SPATIAL_SYSTEM_HPP__
#define __PROCALC_RAYCAST_SPATIAL_SYSTEM_HPP__


#include <string>
#include <list>
#include <map>
#include <set>
#include <memory>
#include <QImage>
#include "raycast/scene_graph.hpp"
#include "raycast/system.hpp"


namespace tinyxml2 { class XMLElement; }
namespace parser { class Object; }


struct Intersection {
  Intersection(CSpatialKind kind)
    : kind(kind) {}

  CSpatialKind kind;
  entityId_t entityId;
  Point point_rel;
  Point point_wld;
  double distanceFromOrigin;
  double distanceAlongTarget;
};

typedef std::unique_ptr<Intersection> pIntersection_t;

struct EChangedZone : public GameEvent {
  EChangedZone(entityId_t entityId, entityId_t oldZone, entityId_t newZone)
    : GameEvent("entityChangedZone"),
      entityId(entityId),
      oldZone(oldZone),
      newZone(newZone) {}

  entityId_t entityId;
  entityId_t oldZone;
  entityId_t newZone;
};

class EntityManager;
class TimeService;

class SpatialSystem : public System {
  public:
    SpatialSystem(EntityManager& entityManager, TimeService& timeService, double frameRate);

    SceneGraph sg;

    void connectZones();

    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;
    virtual void handleEvent(const GameEvent& event,
      const std::set<entityId_t>& entities) override {}

    virtual void addComponent(pComponent_t component) override;
    virtual bool hasComponent(entityId_t entityId) const override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    void moveEntity(entityId_t id, Vec2f dv, double heightAboveFloor = 0);

    std::set<entityId_t> entitiesInRadius(const Point& pos, double radius) const;

    std::list<pIntersection_t> entitiesAlongRay(const CZone& zone, const Point& pos,
      const Vec2f& dir, const Matrix& matrix) const;
    std::list<pIntersection_t> entitiesAlongRay(const Vec2f& dir) const;

    std::list<pIntersection_t> entitiesAlong3dRay(const CZone& zone, const Point& pos,
      double height, const Vec2f& dir, double vAngle, const Matrix& matrix) const;
    std::list<pIntersection_t> entitiesAlong3dRay(const Vec2f& dir, double camSpaceVAngle) const;

    void vRotateCamera(double da);
    void hRotateCamera(double da);
    void movePlayer(const Vec2f& v);

    // TODO: Move this
    void jump();

    virtual ~SpatialSystem() override;

  private:
    EntityManager& m_entityManager;
    TimeService& m_timeService;

    double m_frameRate;

    std::map<entityId_t, CSpatial*> m_components;
    std::map<entityId_t, std::set<entityId_t>> m_entityChildren;

    Vec2i m_playerCell;

    bool isRoot(const CSpatial& c) const;
    void removeEntity_r(entityId_t id);
    void crossZones(SceneGraph& sg, entityId_t entityId, entityId_t oldZone, entityId_t newZone);
    std::pair<Range, Range> getHeightRangeForEntity(entityId_t id) const;

    inline CZone& getCurrentZone() const {
      return dynamic_cast<CZone&>(*m_components.at(sg.player->currentRegion));
    }

    void buoyancy();
    void gravity();
};

void findIntersections_r(const Point& point, const Vec2f& dir, const Matrix& matrix,
  const CZone& zone, std::list<pIntersection_t>& intersections,
  std::set<const CZone*>& visitedZones, std::set<entityId_t>& visitedJoins);

void connectSubzones(CZone& zone);


#endif