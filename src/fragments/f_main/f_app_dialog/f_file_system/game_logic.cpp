#include "fragments/f_main/f_app_dialog/f_file_system/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/agent_system.hpp"
#include "event_system.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "utils.hpp"


using std::string;


namespace going_in_circles {


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(EventSystem& eventSystem, EntityManager& entityManager)
  : m_eventSystem(eventSystem),
    m_entityManager(entityManager) {

  DBG_PRINT("GameLogic::GameLogic\n");

  m_entityId = Component::getNextId();

  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  Player& player = *spatialSystem.sg.player;
  player.invincible = true;

  EventHandlerSystem& eventHandlerSystem
    = m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  CEventHandler* events = new CEventHandler(m_entityId);
  events->broadcastedEventHandlers.push_back(EventHandler{"entity_destroyed",
    std::bind(&GameLogic::onEntityDestroyed, this, std::placeholders::_1)});
  events->broadcastedEventHandlers.push_back(EventHandler{"entity_changed_zone",
    std::bind(&GameLogic::onEntityChangeZone, this, std::placeholders::_1)});
  eventHandlerSystem.addComponent(pComponent_t(events));

  setupLarry(eventHandlerSystem);
}

//===========================================
// GameLogic::setupLarry
//===========================================
void GameLogic::setupLarry(EventHandlerSystem& eventHandlerSystem) {
  auto& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);
  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  auto& agentSystem = m_entityManager.system<AgentSystem>(ComponentKind::C_AGENT);

  entityId_t larryId = Component::getIdFromString("larry");
  CFocus& focus = dynamic_cast<CFocus&>(focusSystem.getComponent(larryId));

  focus.captionText = "\"Life is pleasant here\"";

  auto& larryEvents = dynamic_cast<CEventHandler&>(eventHandlerSystem.getComponent(larryId));
  larryEvents.targetedEventHandlers.push_back(EventHandler{"player_activate_entity",
    [=, &focusSystem](const GameEvent&) {

    focusSystem.showCaption(larryId);
  }});

  entityId_t switchId = Component::getIdFromString("exit_switch");
  CEventHandler* switchEvents = nullptr;

  if (eventHandlerSystem.hasComponent(switchId)) {
    switchEvents = &dynamic_cast<CEventHandler&>(eventHandlerSystem.getComponent(switchId));
  }
  else {
    switchEvents = new CEventHandler(switchId);
    eventHandlerSystem.addComponent(pComponent_t(switchEvents));
  }

  switchEvents->targetedEventHandlers.push_back(EventHandler{"player_activate_entity",
    [=, &spatialSystem, &agentSystem](const GameEvent& e_) {

    entityId_t navPointId = Component::getIdFromString("exit_nav_point");
    auto& vRect = dynamic_cast<CVRect&>(spatialSystem.getComponent(navPointId));

    agentSystem.navigateTo(larryId, vRect.pos);
  }});
}

//===========================================
// GameLogic::onEntityDestroyed
//===========================================
void GameLogic::onEntityDestroyed(const GameEvent& event) {
  auto& e = dynamic_cast<const EEntityDestroyed&>(event);

  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  Player& player = *spatialSystem.sg.player;

  if (e.entityId == player.body) {
    auto& inventorySystem = m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);
    auto& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);

    damageSystem.damageEntity(player.body, -10);

    int ammo = inventorySystem.getBucketValue(player.body, "ammo");
    inventorySystem.subtractFromBucket(player.body, "ammo", ammo);

    entityId_t spawnPointId = Component::getIdFromString("spawn_point");
    CVRect& spawnPoint = dynamic_cast<CVRect&>(spatialSystem.getComponent(spawnPointId));

    spatialSystem.relocateEntity(player.body, *spawnPoint.zone, spawnPoint.pos);
    player.setFeetHeight(0);
  }
}

//===========================================
// GameLogic::onEntityChangeZone
//===========================================
void GameLogic::onEntityChangeZone(const GameEvent& event) {
  const EChangedZone& e = dynamic_cast<const EChangedZone&>(event);

  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  Player& player = *spatialSystem.sg.player;

  if (e.entityId == player.body) {
    if (e.newZone == Component::getIdFromString("level_exit")) {
      m_eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_DOOMSWEEPER)));
    }
  }
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {

}


}
