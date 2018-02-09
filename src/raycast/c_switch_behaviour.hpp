#ifndef __PROCALC_RAYCAST_C_SWITCH_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_SWITCH_BEHAVIOUR_HPP_


#include "raycast/behaviour_system.hpp"
#include "raycast/timing.hpp"


class EntityManager;
class CWallDecal;


enum class SwitchState { ON, OFF };


struct ESwitchActivate : public GameEvent {
  ESwitchActivate(SwitchState state)
    : GameEvent("switchActivate"),
      state(state) {}

  SwitchState state;
};

class CSwitchBehaviour : public CBehaviour {
  public:
    CSwitchBehaviour(entityId_t entityId, EntityManager& entityManager, entityId_t target,
      bool toggleable, double toggleDelay);

    virtual void update() override;
    virtual void handleEvent(const GameEvent& e) override;

    virtual ~CSwitchBehaviour() override {}

  private:
    EntityManager& m_entityManager;
    entityId_t m_target;
    SwitchState m_state = SwitchState::OFF;
    bool m_toggleable;
    double m_toggleDelay;
    Debouncer m_timer;

    CWallDecal* getDecal() const;
};


#endif
