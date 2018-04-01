#include "fragments/f_main/f_app_dialog/f_server_room/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "event_system.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "utils.hpp"


using std::string;


namespace youve_got_mail {


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(EventSystem& eventSystem, EntityManager& entityManager)
  : m_eventSystem(eventSystem),
    m_entityManager(entityManager) {

  DBG_PRINT("GameLogic::GameLogic\n");

  m_eventSystem.listen("youveGotMail/divByZero", [this](const Event& event) {
    onDivByZero(event);
  }, m_eventIdx);

  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  const Player& player = *spatialSystem.sg.player;

  entityId_t entityId = Component::getNextId();

  CEventHandler* handlers = new CEventHandler(entityId);
  handlers->broadcastedEventHandlers.push_back(EventHandler{"entity_changed_zone",
    [=, &player](const GameEvent& e_) {

    const EChangedZone& e = dynamic_cast<const EChangedZone&>(e_);
    if (e.entityId == player.body && e.newZone == Component::getIdFromString("level_exit")) {
      m_eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_SUICIDE_MISSION, true)));
    }
  }});

  eventHandlerSystem.addComponent(pComponent_t(handlers));
}

//===========================================
// GameLogic::onDivByZero
//===========================================
void GameLogic::onDivByZero(const Event& event) {
  m_entityManager.broadcastEvent(GameEvent("div_by_zero"));
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {
  m_eventSystem.forget(m_eventIdx);
}


}
