#ifndef __PROCALC_RAYCAST_C_DOOR_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_DOOR_BEHAVIOUR_HPP_


#include "raycast/behaviour_system.hpp"
#include "raycast/timing.hpp"


class EntityManager;

class CDoorBehaviour : public CBehaviour {
  public:
    CDoorBehaviour(entityId_t entityId, EntityManager& entityManager, double frameRate);

    virtual void update() override;
    virtual void handleEvent(const GameEvent& e) override;

  private:
    enum state_t {
      ST_OPENING,
      ST_CLOSING,
      ST_OPEN,
      ST_CLOSED
    };

    EntityManager& m_entityManager;
    double m_frameRate;
    state_t m_state = ST_CLOSED;
    double m_y0;
    double m_y1;
    Debouncer m_timer;
};


#endif
