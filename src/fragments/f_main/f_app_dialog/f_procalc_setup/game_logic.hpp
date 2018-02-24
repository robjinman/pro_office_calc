#ifndef __PROCALC_FRAGMENTS_F_PROCALC_SETUP_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_PROCALC_SETUP_GAME_LOGIC_HPP__



class EventSystem;
class EntityManager;
class GameEvent;

namespace making_progress {


class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager);

    ~GameLogic();

  private:
    void onElevatorStopped(const GameEvent& event);

    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    int m_eventIdx = -1;
};


}



#endif
