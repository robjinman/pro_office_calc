#ifndef __PROCALC_RAYCAST_C_SWITCH_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_SWITCH_BEHAVIOUR_HPP_


#include <string>
#include "raycast/behaviour_system.hpp"
#include "raycast/timing.hpp"


class EntityManager;
class TimeService;
class CWallDecal;


enum class SwitchState { ON, OFF };


class ESwitchActivate : public GameEvent {
  public:
    ESwitchActivate(entityId_t switchEntityId, SwitchState state, const std::string& message)
      : GameEvent("switch_activated"),
        switchEntityId(switchEntityId),
        message(message),
        state(state) {}

    entityId_t switchEntityId;
    std::string message;
    SwitchState state;
};

class CSwitchBehaviour : public CBehaviour {
  public:
    CSwitchBehaviour(entityId_t entityId, EntityManager& entityManager, TimeService& TimeService,
      entityId_t target, const std::string& message, SwitchState initialState, bool toggleable,
      double toggleDelay);

    std::string requiredItemType;
    std::string requiredItemName;

    void update() override;
    void handleBroadcastedEvent(const GameEvent& event) override {}
    void handleTargetedEvent(const GameEvent& event) override;

    ~CSwitchBehaviour() override {}

  private:
    EntityManager& m_entityManager;
    TimeService& m_timeService;
    entityId_t m_target = -1;
    std::string m_message;
    SwitchState m_state;
    bool m_toggleable;
    Debouncer m_timer;

    CWallDecal* getDecal() const;
    void setDecal();

    void showCaption();
    void deleteCaption();
};


#endif
