#include <QDialog>
#include <QApplication>
#include "fragments/f_main/f_app_dialog/f_procalc_setup/game_logic.hpp"
#include "fragments/f_main/f_app_dialog/f_procalc_setup/setup_complete_event.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/c_switch_behaviour.hpp"
#include "raycast/c_elevator_behaviour.hpp"
#include "raycast/spatial_system.hpp"
#include "event_system.hpp"
#include "state_ids.hpp"
#include "request_state_change_event.hpp"
#include "utils.hpp"


using std::set;
using std::string;


namespace making_progress {


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(QDialog& dialog, EventSystem& eventSystem, EntityManager& entityManager)
  : m_dialog(dialog),
    m_eventSystem(eventSystem),
    m_entityManager(entityManager),
    m_entityId(Component::getNextId()),
    m_raiseDialogEvent(static_cast<QEvent::Type>(QEvent::registerEventType())) {

  DBG_PRINT("GameLogic::GameLogic\n");

  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  pCEventHandler_t forwardEvent(new CEventHandler(m_entityId));

  forwardEvent->handlers.push_back(EventHandler{"entityChangedZone",
    std::bind(&GameLogic::onEntityChangeZone, this, std::placeholders::_1)});

  eventHandlerSystem.addComponent(std::move(forwardEvent));

  m_eventSystem.listen("makingProgress", [](const Event& event) {
    // TODO
  }, m_eventIdx);
}

//===========================================
// GameLogic::setFeatures
//===========================================
void GameLogic::setFeatures(const set<buttonId_t>& features) {
  m_features = features;

  entityId_t elevatorId = Component::getIdFromString("progress_lift");

  CElevatorBehaviour& elevator =
    m_entityManager.getComponent<CElevatorBehaviour>(elevatorId, ComponentKind::C_BEHAVIOUR);

  const double MIN_SPEED = 2;
  const double MAX_SPEED = 60;
  const double MAX_FEATURES = 17;
  double delta = (MAX_SPEED - MIN_SPEED) / MAX_FEATURES;

  elevator.setSpeed(MAX_SPEED - delta * m_features.size());
}

//===========================================
// GameLogic::customEvent
//===========================================
void GameLogic::customEvent(QEvent* event) {
  if (event->type() == m_raiseDialogEvent) {
    m_dialog.activateWindow();
  }
}

//===========================================
// GameLogic::onEntityChangeZone
//===========================================
void GameLogic::onEntityChangeZone(const GameEvent& event) {
  const EChangedZone& e = dynamic_cast<const EChangedZone&>(event);

  entityId_t player =
    m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL).sg.player->body;

  if (e.entityId != player) {
    return;
  }

  if (e.newZone == Component::getIdFromString("puzzle_room_entrance")) {
    m_eventSystem.fire(pEvent_t(new SetupCompleteEvent(m_features)));

    QApplication::postEvent(this, new QEvent(m_raiseDialogEvent));
  }
  else if (e.newZone == Component::getIdFromString("level_exit")) {
    m_eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_YOUVE_GOT_MAIL)));
  }
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {
  m_eventSystem.forget(m_eventIdx);
  m_entityManager.deleteEntity(m_entityId);
}


}
