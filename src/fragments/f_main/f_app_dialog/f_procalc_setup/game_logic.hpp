#ifndef __PROCALC_FRAGMENTS_F_PROCALC_SETUP_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_PROCALC_SETUP_GAME_LOGIC_HPP__


#include <set>
#include <QObject>
#include <QEvent>
#include "button_grid.hpp"
#include "event_system.hpp"
#include "raycast/component.hpp"


class Event;
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

    void onButtonPress(const Event& event);

    void setElevatorSpeed();
    void generateTargetNumber();

    QDialog& m_dialog;
    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    entityId_t m_entityId;
    pEventHandle_t m_hButtonPress;
    QEvent::Type m_raiseDialogEvent;
    double m_targetNumber = 0;
    int m_numKeysPressed = 0;
    bool m_success = false;

    std::set<buttonId_t> m_features;
};


}



#endif
