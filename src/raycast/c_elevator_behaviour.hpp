#ifndef __PROCALC_RAYCAST_C_ELEVATOR_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_ELEVATOR_BEHAVIOUR_HPP_


#include <vector>
#include "raycast/behaviour_system.hpp"


class EntityManager;

class CElevatorBehaviour : public CBehaviour {
  public:
    CElevatorBehaviour(entityId_t entityId, EntityManager& entityManager, double frameRate,
      const std::vector<double>& levels);

    virtual void update() override;
    virtual void handleEvent(const GameEvent& e) override;

    void setSpeed(double speed);

  private:
    enum state_t {
      ST_STOPPED,
      ST_MOVING
    };

    EntityManager& m_entityManager;
    double m_frameRate;
    state_t m_state = ST_STOPPED;
    int m_target = 0;
    double m_speed = 60.0;
    std::vector<double> m_levels;
};


#endif