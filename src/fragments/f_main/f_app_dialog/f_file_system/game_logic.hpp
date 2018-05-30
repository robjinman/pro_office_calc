#ifndef __PROCALC_FRAGMENTS_F_FILE_SYSTEM_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_FILE_SYSTEM_GAME_LOGIC_HPP__


#include <string>


class EventSystem;
class Event;
class EntityManager;


namespace going_in_circles {


class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager);
    GameLogic(const GameLogic& cpy) = delete;

    ~GameLogic();

  private:
    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
};


}



#endif
