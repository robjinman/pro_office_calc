#ifndef __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_GAME_LOGIC_HPP__


#include "raycast/component.hpp"


class EventSystem;
class EntityManager;
class GameEvent;

namespace its_raining_tetrominos {


class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager);

    ~GameLogic();

  private:
    void onSwitchActivate(const GameEvent& event);
    void onChangeZone(const GameEvent& event);

    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    entityId_t m_entityId;
    int m_eventIdx = -1;
};


}



#endif
