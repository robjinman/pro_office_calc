#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_C_ENEMY_BEHAVIOUR_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_C_ENEMY_BEHAVIOUR_HPP_


#include <vector>
#include <random>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/timing.hpp"


class EntityManager;
class AudioService;
class TimeService;
class SpatialSystem;
class CVRect;

class CEnemyBehaviour : public CBehaviour {
  public:
    CEnemyBehaviour(entityId_t entityId, EntityManager& entityManager, AudioService& audioService,
      TimeService& timeService);

    virtual void update() override;
    virtual void handleEvent(const GameEvent& e) override;

    std::vector<Point> patrolPath;
    entityId_t stPatrollingTrigger = -1;
    entityId_t stChasingTrigger = -1;

  private:
    enum state_t {
      ST_IDLE,
      ST_PATROLLING,
      ST_CHASING,
      ST_SHOOTING
    };

    std::mt19937 m_randEngine;

    EntityManager& m_entityManager;
    AudioService& m_audioService;
    TimeService& m_timeService;
    state_t m_state = ST_IDLE;
    bool m_shooting = false;

    int m_waypointIdx = -1;

    std::unique_ptr<TimePattern> m_gunfireTiming;

    void doPatrollingBehaviour(SpatialSystem& spatialSystem, CVRect& body);
    void doChasingBehaviour(SpatialSystem& spatialSystem, CVRect& body);
    void attemptShot(SpatialSystem& spatialSystem, CVRect& body);
};


#endif
