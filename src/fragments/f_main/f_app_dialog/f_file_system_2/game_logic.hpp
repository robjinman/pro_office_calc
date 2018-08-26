#ifndef __PROCALC_FRAGMENTS_F_FILE_SYSTEM_2_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_FILE_SYSTEM_2_GAME_LOGIC_HPP__


#include <string>
#include "raycast/component.hpp"
#include "raycast/system_accessor.hpp"


class EventSystem;
class Event;
class EntityManager;
class AudioService;
class TimeService;
class EventHandlerSystem;
class GameEvent;


namespace t_minus_two_minutes {


class GameLogic : private SystemAccessor {
  public:
    GameLogic(EventSystem& eventSystem, AudioService& audioService, TimeService& timeService,
      EntityManager& entityManager);
    GameLogic(const GameLogic& cpy) = delete;

    ~GameLogic();

  private:
    void useCovfefe();
    void setupTimer();
    void updateTimer();
    void fadeToBlack();
    void onMidnight();

    EventSystem& m_eventSystem;
    AudioService& m_audioService;
    TimeService& m_timeService;
    EntityManager& m_entityManager;

    entityId_t m_entityId = -1;
    long m_timerHandle = -1;
    long m_fadeOutHandle = -1;
    int m_timeRemaining = 120;
    bool m_missionAccomplished = false;
};


}



#endif
