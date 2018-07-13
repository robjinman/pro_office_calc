#ifndef __PROCALC_FRAGMENTS_F_FILE_SYSTEM_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_FILE_SYSTEM_GAME_LOGIC_HPP__


#include <string>
#include "raycast/component.hpp"


class EventSystem;
class Event;
class EntityManager;
class AudioService;
class TimeService;
class EventHandlerSystem;
class GameEvent;
class CSwitchBehaviour;


namespace going_in_circles {


class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, AudioService& audioService, TimeService& timeService,
      EntityManager& entityManager);
    GameLogic(const GameLogic& cpy) = delete;

    ~GameLogic();

  private:
    void onEntityDestroyed(const GameEvent& event);
    void onEntityChangeZone(const GameEvent& event);
    void setupLarry(EventHandlerSystem& eventHandlerSystem);
    void onSwitchActivated(const GameEvent& event);
    void resetSwitches();
    CSwitchBehaviour& getSwitch(entityId_t) const;

    EventSystem& m_eventSystem;
    AudioService& m_audioService;
    TimeService& m_timeService;
    EntityManager& m_entityManager;

    entityId_t m_entityId = -1;
};


}



#endif
