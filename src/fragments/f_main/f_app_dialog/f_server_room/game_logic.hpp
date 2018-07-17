#ifndef __PROCALC_FRAGMENTS_F_SERVER_ROOM_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_SERVER_ROOM_GAME_LOGIC_HPP__


#include <string>
#include "event_system.hpp"


class Event;
class EntityManager;

namespace youve_got_mail {


class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager);
    GameLogic(const GameLogic& cpy) = delete;

    ~GameLogic();

  private:
    void onDivByZero(const Event& event);
    void drawExitDoorDigitDisplay();

    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    EventHandle m_hDivByZero;
    EventHandle m_hDialogClosed;
    char m_exitDoorSelectedNum = '\0';
    std::string m_exitDoorInput;
};


}



#endif
