#ifndef __PROCALC_RAYCAST_C_SWITCH_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_SWITCH_BEHAVIOUR_HPP_


#include <string>
#include "raycast/behaviour_system.hpp"
#include "raycast/timing.hpp"


class EntityManager;
class CWallDecal;


enum class SwitchState { ON, OFF };


struct ESwitchActivate : public GameEvent {
  ESwitchActivate(entityId_t switchEntityId, SwitchState state, const std::string& message)
    : GameEvent("switch_activated"),
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

    std::string requiredItemType;
    std::string requiredItemName;

    void update() override;
    void handleBroadcastedEvent(const GameEvent& event) override {}
    void handleTargetedEvent(const GameEvent& event) override;

    ~CSwitchBehaviour() override {}

  private:
    CWallDecal* getDecal() const;
    void setDecal();

    EntityManager& m_entityManager;
    entityId_t m_target = -1;
    std::string m_message;
    SwitchState m_state;
    bool m_toggleable;
    double m_toggleDelay;
    Debouncer m_timer;
};


#endif
