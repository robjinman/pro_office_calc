#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SPATIAL_SYSTEM_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SPATIAL_SYSTEM_HPP__


#include <string>
#include <list>
#include <map>
#include <set>
#include <memory>
#include <QImage>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_graph.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"


namespace tinyxml2 { class XMLElement; }
namespace parser { class Object; }


struct Tween {
  std::function<bool()> tick;
  std::function<void()> finish;
};

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

class SpatialSystem : public System {
  public:
    SpatialSystem(EntityManager& entityManager, double frameRate);

    SceneGraph sg;

    void connectZones();

    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;

    virtual void addComponent(pComponent_t component) override;
    virtual bool hasComponent(entityId_t entityId) const override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    void moveEntity(entityId_t id, Vec2f dv, double heightAboveFloor = 0);

    std::set<entityId_t> entitiesInRadius(const Point& pos, double radius) const;

    std::list<pIntersection_t> entitiesAlongRay(const CZone& zone, const Point& pos, double angle,
      const Matrix& matrix) const;
    std::list<pIntersection_t> entitiesAlongRay(double camSpaceAngle) const;

    std::list<pIntersection_t> entitiesAlong3dRay(const CZone& zone, const Point& pos,
      double hAngle, double vAngle, const Matrix& matrix) const;
    std::list<pIntersection_t> entitiesAlong3dRay(double camSpaceHAngle,
      double camSpaceVAngle) const;

    void vRotateCamera(double da);
    void hRotateCamera(double da);
    void movePlayer(const Vec2f& v);

    // TODO: Move this
    void jump();

    // TODO: Move this
    void addTween(const Tween& tween, const char* name = nullptr);

    virtual ~SpatialSystem() override;

  private:
    EntityManager& m_entityManager;

    double m_frameRate;
    std::map<std::string, Tween> m_tweens;

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


#endif
