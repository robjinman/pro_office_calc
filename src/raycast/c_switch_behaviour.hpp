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
      const std::string& message, SwitchState initialState, bool toggleable, double toggleDelay);

    std::string requiredItemType;
    std::string requiredItemName;
    bool disabled = false;

    entityId_t target = -1;

    void update() override;
    void handleBroadcastedEvent(const GameEvent& event) override {}
    void handleTargetedEvent(const GameEvent& event) override;

    void setState(SwitchState state);
    SwitchState getState() const;

    ~CSwitchBehaviour() override {}

  private:
    EntityManager& m_entityManager;
    TimeService& m_timeService;
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
