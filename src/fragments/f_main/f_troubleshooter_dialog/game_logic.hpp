#ifndef __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_GAME_LOGIC_HPP__



class EventSystem;
class EntityManager;
class GameEvent;

namespace its_raining_tetrominos {


class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager);

    ~GameLogic();

  private:
    void onRaycastEvent(const GameEvent& event);

    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    int m_eventIdx;
};


}



#endif
