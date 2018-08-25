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
#include "raycast/system_accessor.hpp"


class TimeService;
class AudioService;
class DamageSystem;
class SpatialSystem;
class AgentSystem;
class CVRect;
class EntityManager;

class CAgent : public Component, private SystemAccessor {
  friend class AgentSystem;

  public:
    CAgent(entityId_t entityId, EntityManager& entityManager);

    bool isHostile = true;
    entityId_t patrolPath = -1;
    std::string stPatrollingTrigger;
    std::string stChasingTrigger;

    virtual ~CAgent() override {}

  private:
    enum state_t {
      ST_STATIONARY,
      ST_ON_FIXED_PATH,
      ST_CHASING_OBJECT,
      ST_SHOOTING
    };

    EntityManager& m_entityManager;

    state_t m_state = ST_STATIONARY;
    std::unique_ptr<TimePattern> m_gunfireTiming;
    entityId_t m_targetObject = -1;
    std::vector<Point> m_path;
    bool m_pathClosed = true;
    int m_waypointIdx = -1;
    std::function<void(CAgent&)> m_onFinish;

    void navigateTo(const Point& p, std::function<void(CAgent&)> onFinish);
    void startPatrol();
    void startChase();

    bool hasLineOfSight(Matrix& m, Vec2f& ray, double& hAngle, double& vAngle,
      double& height) const;

    void followPath(TimeService& timeService);
    void attemptShot(TimeService& timeService, AudioService& audioService);

    virtual void update(TimeService& timeService, AudioService& audioService);

    void handleEvent(const GameEvent& event);

    void onDamage();

    void setAnimation();
    void setState(state_t state);
};

typedef std::unique_ptr<CAgent> pCAgent_t;

class EntityManager;

class AgentSystem : public System {
  public:
    AgentSystem(EntityManager& entityManager, TimeService& timeService, AudioService& audioService)
      : m_entityManager(entityManager),
        m_timeService(timeService),
        m_audioService(audioService) {}

    void update() override;
    void handleEvent(const GameEvent& event) override;
    void handleEvent(const GameEvent&, const std::set<entityId_t>&) override {}

    void addComponent(pComponent_t component) override;
    bool hasComponent(entityId_t entityId) const override;
    CAgent& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

    void navigateTo(entityId_t entityId, const Point& point);

  private:
    EntityManager& m_entityManager;
    TimeService& m_timeService;
    AudioService& m_audioService;
    std::map<entityId_t, pCAgent_t> m_components;
};


#endif
