#ifndef __PROCALC_FRAGMENTS_F_PROCALC_SETUP_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_PROCALC_SETUP_GAME_LOGIC_HPP__


#include <set>
#include <QObject>
#include <QEvent>
#include "button_grid.hpp"
#include "raycast/component.hpp"


class EventSystem;
class EntityManager;
class GameEvent;

namespace making_progress {


class GameLogic : public QObject {
  Q_OBJECT

  public:
    GameLogic(QDialog& dialog, EventSystem& eventSystem, EntityManager& entityManager);
    GameLogic(const GameLogic& cpy) = delete;

    void setFeatures(const std::set<buttonId_t>& features);

    ~GameLogic();

  protected:
    void customEvent(QEvent* event) override;

  private:
    void onElevatorStopped(const GameEvent& event);
    void onEntityChangeZone(const GameEvent& event);

    QDialog& m_dialog;
    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    entityId_t m_entityId;
    int m_eventIdx = -1;
    QEvent::Type m_raiseDialogEvent;

    std::set<buttonId_t> m_features;
};


}



#endif
