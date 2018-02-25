#ifndef __PROCALC_FRAGMENTS_F_PROCALC_SETUP_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_PROCALC_SETUP_GAME_LOGIC_HPP__


#include <set>
#include "button_grid.hpp"
#include "raycast/component.hpp"


class EventSystem;
class EntityManager;
class GameEvent;

namespace making_progress {


class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager);
    GameLogic(const GameLogic& cpy) = delete;

    void setFeatures(const std::set<buttonId_t>& features);

    ~GameLogic();

  private:
    void onElevatorStopped(const GameEvent& event);

    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    entityId_t m_entityId;
    int m_eventIdx = -1;

    std::set<buttonId_t> m_features;
};


}



#endif
