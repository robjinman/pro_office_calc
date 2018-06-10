#include "fragments/f_main/f_app_dialog/f_file_system/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/inventory_system.hpp"
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

      player.setFeetHeight(100);
      spatialSystem.relocateEntity(player.body, *spawnPoint.zone, spawnPoint.pos);
    }
  }});

  eventHandlerSystem.addComponent(pComponent_t(events));
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {

}


}
