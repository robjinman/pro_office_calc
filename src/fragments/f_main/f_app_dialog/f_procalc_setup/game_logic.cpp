#include "fragments/f_main/f_app_dialog/f_procalc_setup/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/c_switch_behaviour.hpp"
#include "event_system.hpp"
#include "state_ids.hpp"
#include "request_state_change_event.hpp"
#include "utils.hpp"


using std::string;


namespace making_progress {


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(EventSystem& eventSystem, EntityManager& entityManager)
  : m_eventSystem(eventSystem),
    m_entityManager(entityManager) {

  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  pCEventHandler_t forwardEvent(new CEventHandler(Component::getNextId()));

  forwardEvent->handlers.push_back(EventHandler{"*", std::bind(&GameLogic::onRaycastEvent,
    *this, std::placeholders::_1)});

  eventHandlerSystem.addComponent(std::move(forwardEvent));

  m_eventSystem.listen("procalc_setup", [](const Event& event) {
    // TODO
  }, m_eventIdx);
}

//===========================================
// GameLogic::onRaycastEvent
//===========================================
void GameLogic::onRaycastEvent(const GameEvent& event) {

}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {
  m_eventSystem.forget(m_eventIdx);
}


}
