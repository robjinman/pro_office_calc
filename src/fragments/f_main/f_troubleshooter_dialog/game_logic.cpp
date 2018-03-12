#include "fragments/f_main/f_troubleshooter_dialog/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/c_switch_behaviour.hpp"
#include "event_system.hpp"
#include "state_ids.hpp"
#include "request_state_change_event.hpp"
#include "utils.hpp"


using std::string;


namespace its_raining_tetrominos {


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(EventSystem& eventSystem, EntityManager& entityManager)
  : m_eventSystem(eventSystem),
    m_entityManager(entityManager),
    m_entityId(Component::getNextId()) {

  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  pCEventHandler_t forwardEvent(new CEventHandler(m_entityId));

  forwardEvent->handlers.push_back(EventHandler{"*", std::bind(&GameLogic::onRaycastEvent,
    this, std::placeholders::_1)});

  eventHandlerSystem.addComponent(std::move(forwardEvent));

  m_eventSystem.listen("its_raining_tetrominos", [](const Event& event) {
    // TODO
  }, m_eventIdx);
}

//===========================================
// GameLogic::onRaycastEvent
//===========================================
void GameLogic::onRaycastEvent(const GameEvent& event) {
  if (event.name == "switch_activated") {
    const auto& e = dynamic_cast<const ESwitchActivate&>(event);

    if (e.state == SwitchState::ON) {
      // TODO: Cool transition

      m_eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_OFFICE_ASSISTANT)));
    }
    else {
      m_entityManager.broadcastEvent(GameEvent("machine_deactivated"));
    }
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
