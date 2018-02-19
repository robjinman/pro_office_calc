#ifndef __PROCALC_RAYCAST_C_SWITCH_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_SWITCH_BEHAVIOUR_HPP_


#include <string>
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

struct ESwitchActivateEntity : public GameEvent {
  ESwitchActivateEntity(entityId_t switchEntityId, SwitchState state, const std::string& message)
    : GameEvent("switchActivateEntity"),
      switchEntityId(switchEntityId),
      state(state),
      message(message) {}

  entityId_t switchEntityId;
  SwitchState state;
  std::string message;
};

class CSwitchBehaviour : public CBehaviour {
  public:
    CSwitchBehaviour(entityId_t entityId, EntityManager& entityManager, entityId_t target,
      const std::string& message, SwitchState initialState, bool toggleable, double toggleDelay);

    virtual void update() override;
    virtual void handleEvent(const GameEvent& e) override;

    virtual ~CSwitchBehaviour() override {}

  private:
    CWallDecal* getDecal() const;
    void setDecal();

    EntityManager& m_entityManager;
    entityId_t m_target;
    std::string m_message;
    SwitchState m_state;
    bool m_toggleable;
    double m_toggleDelay;
    Debouncer m_timer;
};


#endif
