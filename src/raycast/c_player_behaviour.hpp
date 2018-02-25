#ifndef __PROCALC_RAYCAST_C_PLAYER_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_PLAYER_BEHAVIOUR_HPP_


#include <vector>
#include "raycast/behaviour_system.hpp"


class EntityManager;
class TimeService;

class CPlayerBehaviour : public CBehaviour {
  public:
    CPlayerBehaviour(entityId_t entityId, EntityManager& entityManager, TimeService& timeService);

    virtual void update() override;
    virtual void handleEvent(const GameEvent& e) override;

  private:
    enum state_t {
      ST_ALIVE,
      ST_DEAD
    };

    EntityManager& m_entityManager;
    TimeService& m_timeService;
    state_t m_state = ST_ALIVE;
};


#endif
