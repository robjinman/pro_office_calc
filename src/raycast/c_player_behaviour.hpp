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
    virtual void handleBroadcastedEvent(const GameEvent&) override {}
    virtual void handleTargetedEvent(const GameEvent& e) override;

  private:
    EntityManager& m_entityManager;
    TimeService& m_timeService;
};


#endif
