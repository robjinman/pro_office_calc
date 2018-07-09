#include "fragments/f_main/f_troubleshooter_dialog/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/c_switch_behaviour.hpp"
#include "event_system.hpp"
#include "app_config.hpp"
#include "state_ids.hpp"
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

  CEventHandler* events = new CEventHandler(m_entityId);

  events->broadcastedEventHandlers.push_back(EventHandler{"switch_activated",
    std::bind(&GameLogic::onSwitchActivate, this, std::placeholders::_1)});
  events->broadcastedEventHandlers.push_back(EventHandler{"entity_changed_zone",
    std::bind(&GameLogic::onChangeZone, this, std::placeholders::_1)});

  eventHandlerSystem.addComponent(pComponent_t(events));
}

//===========================================
// GameLogic::onChangeZone
//===========================================
void GameLogic::onChangeZone(const GameEvent& e_) {
  static entityId_t playerId = Component::getIdFromString("player");
  static entityId_t doorId = Component::getIdFromString("exit_door");

  const auto& e = dynamic_cast<const EChangedZone&>(e_);

  if (e.entityId == playerId && e.newZone == doorId) {
    m_eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_MAKING_PROGRESS)));
  }
}

//===========================================
// GameLogic::onSwitchActivate
//===========================================
void GameLogic::onSwitchActivate(const GameEvent& e_) {
  static entityId_t doorId = Component::getIdFromString("exit_door");

  const auto& e = dynamic_cast<const ESwitchActivate&>(e_);

  if (e.state == SwitchState::ON) {
    // TODO: Cool transition

    m_entityManager.fireEvent(EActivateEntity{doorId}, {doorId});
  }
  else {
    m_entityManager.broadcastEvent(GameEvent("machine_deactivated"));
  }
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {
  m_entityManager.deleteEntity(m_entityId);
}


}
