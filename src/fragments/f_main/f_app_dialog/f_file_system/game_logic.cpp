#include "fragments/f_main/f_app_dialog/f_file_system/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/focus_system.hpp"
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

  auto& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  auto& inventorySystem = m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);
  auto& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);

  Player& player = *spatialSystem.sg.player;

  player.invincible = true;

  EventHandlerSystem& eventHandlerSystem
    = m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  CEventHandler* events = new CEventHandler(m_entityId);
  events->broadcastedEventHandlers.push_back(EventHandler{"entity_destroyed",
    [=, &player, &damageSystem, &spatialSystem, &inventorySystem](const GameEvent& e_) {

    auto& e = dynamic_cast<const EEntityDestroyed&>(e_);

    if (e.entityId == player.body) {
      std::cout << "Player is dead!\n";

      damageSystem.damageEntity(player.body, -10);

      int ammo = inventorySystem.getBucketValue(player.body, "ammo");
      inventorySystem.subtractFromBucket(player.body, "ammo", ammo);

      entityId_t spawnPointId = Component::getIdFromString("spawn_point");
      CVRect& spawnPoint = dynamic_cast<CVRect&>(spatialSystem.getComponent(spawnPointId));

      spatialSystem.relocateEntity(player.body, *spawnPoint.zone, spawnPoint.pos);
      player.setFeetHeight(0);
    }
  }});

  eventHandlerSystem.addComponent(pComponent_t(events));

  entityId_t larryId = Component::getIdFromString("larry");
  CFocus& focus = dynamic_cast<CFocus&>(focusSystem.getComponent(larryId));

  focus.captionText = "\"Life is pleasant here\"";

  auto& larryEvents = dynamic_cast<CEventHandler&>(eventHandlerSystem.getComponent(larryId));
  larryEvents.targetedEventHandlers.push_back(EventHandler{"player_activate_entity",
    [=, &focusSystem](const GameEvent&) {

    focusSystem.showCaption(larryId);
  }});
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {

}


}
