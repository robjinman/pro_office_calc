#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_C_ENEMY_BEHAVIOUR_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_C_ENEMY_BEHAVIOUR_HPP_


#include <vector>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"


class EntityManager;

class CEnemyBehaviour : public CBehaviour {
  public:
    CEnemyBehaviour(entityId_t entityId, EntityManager& entityManager, double frameRate);

    virtual void update() override;
    virtual void handleEvent(const GameEvent& e) override;

    std::vector<Point> patrolPath;

  private:
    enum state_t {
      ST_PATROLLING,
      ST_CHASING
    };

    EntityManager& m_entityManager;
    double m_frameRate;
    state_t m_state = ST_PATROLLING;

    int m_waypointIdx = -1;
};


#endif
