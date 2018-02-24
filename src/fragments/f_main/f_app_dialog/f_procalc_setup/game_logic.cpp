#include "fragments/f_main/f_app_dialog/f_procalc_setup/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/c_switch_behaviour.hpp"
#include "raycast/c_elevator_behaviour.hpp"
#include "event_system.hpp"
#include "state_ids.hpp"
#include "request_state_change_event.hpp"
#include "utils.hpp"


using std::string;


namespace making_progress {


static const std::string ELEVATOR_NAME = "progress_lift";


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(EventSystem& eventSystem, EntityManager& entityManager)
  : m_eventSystem(eventSystem),
    m_entityManager(entityManager) {

  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  pCEventHandler_t forwardEvent(new CEventHandler(Component::getNextId()));

  forwardEvent->handlers.push_back(EventHandler{"elevatorStopped",
    std::bind(&GameLogic::onElevatorStopped, *this, std::placeholders::_1)});

  eventHandlerSystem.addComponent(std::move(forwardEvent));

  m_eventSystem.listen("makingProgress", [](const Event& event) {
    // TODO
  }, m_eventIdx);
}

//===========================================
// GameLogic::onElevatorStopped
//===========================================
void GameLogic::onElevatorStopped(const GameEvent& event) {
  const EElevatorStopped& e = dynamic_cast<const EElevatorStopped&>(event);

  if (e.entityId == Component::getIdFromString(ELEVATOR_NAME)) {
    m_eventSystem.fire(pEvent_t(new Event("makingProgress/setupComplete")));
  }
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {
  m_eventSystem.forget(m_eventIdx);
}


}
