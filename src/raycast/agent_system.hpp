#ifndef __PROCALC_RAYCAST_AGENT_SYSTEM_HPP_
#define __PROCALC_RAYCAST_AGENT_SYSTEM_HPP_


#include <memory>
#include <map>
#include <functional>
#include <vector>
#include "raycast/system.hpp"
#include "raycast/component.hpp"
#include "raycast/geometry.hpp"
#include "raycast/timing.hpp"


class TimeService;
class AudioService;
class DamageSystem;
class SpatialSystem;
class AgentSystem;
class CVRect;

class CAgent : public Component {
  friend class AgentSystem;

  public:
    CAgent(entityId_t entityId);

    std::vector<Point> patrolPath;
    entityId_t stPatrollingTrigger = -1;
    entityId_t stChasingTrigger = -1;

    virtual ~CAgent() override {}

  private:
    enum state_t {
      ST_STATIONARY,
      ST_ON_FIXED_PATH,
      ST_CHASING_OBJECT
    };

    state_t m_state = ST_STATIONARY;

    bool m_shooting = false;
    std::unique_ptr<TimePattern> m_gunfireTiming;

    entityId_t m_targetObject = -1;
    std::vector<Point> m_path;
    bool m_pathClosed = true;
    int m_waypointIdx = -1;
    std::function<void(CAgent&)> m_onFinish;

    void navigateTo(SpatialSystem& spatialSystem, const Point& p,
      std::function<void(CAgent&)> onFinish);
    void startPatrol();

    bool hasLineOfSight(SpatialSystem& spatialSystem, Matrix& m, Vec2f& ray, double& hAngle,
      double& vAngle, double& height) const;

    void followPath(SpatialSystem& spatialSystem, TimeService& timeService);
    void attemptShot(AgentSystem& agentSystem, SpatialSystem& spatialSystem,
      DamageSystem& damageSystem, TimeService& timeService, AudioService& audioService);

    virtual void update(AgentSystem& agentSystem, SpatialSystem& spatialSystem,
      DamageSystem& damageSystem, TimeService& timeService, AudioService& audioService);

    void onPlayerChangeZone(entityId_t newZone, SpatialSystem& spatialSystem);
    void onDamage(SpatialSystem& spatialSystem);
};

typedef std::unique_ptr<CAgent> pCAgent_t;

class EntityManager;

class AgentSystem : public System {
  public:
    AgentSystem(EntityManager& entityManager, TimeService& timeService, AudioService& audioService)
      : m_entityManager(entityManager),
        m_timeService(timeService),
        m_audioService(audioService) {}

    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;
    virtual void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) override {} // TODO

    virtual void addComponent(pComponent_t component) override;
    virtual bool hasComponent(entityId_t entityId) const override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    void navigateTo(entityId_t entityId, const Point& point);

    virtual ~AgentSystem() override {}

  private:
    EntityManager& m_entityManager;
    TimeService& m_timeService;
    AudioService& m_audioService;
    std::map<entityId_t, pCAgent_t> m_components;
};


#endif
