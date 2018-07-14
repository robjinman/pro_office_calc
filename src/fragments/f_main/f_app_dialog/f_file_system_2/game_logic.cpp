#include "fragments/f_main/f_app_dialog/f_file_system_2/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/audio_service.hpp"
#include "raycast/time_service.hpp"
#include "event_system.hpp"
#include "state_ids.hpp"
#include "utils.hpp"
#include "app_config.hpp"


using std::string;
using std::map;
using std::set;
using std::vector;


namespace t_minus_two_minutes {


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(EventSystem& eventSystem, AudioService& audioService, TimeService& timeService,
  EntityManager& entityManager)
  : m_eventSystem(eventSystem),
    m_audioService(audioService),
    m_timeService(timeService),
    m_entityManager(entityManager) {

  DBG_PRINT("GameLogic::GameLogic\n");

  auto& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
  auto& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);

  m_entityId = Component::getNextId();

  CFocus* focus = new CFocus(m_entityId);
  focus->captionText = "The covfefe has jammed the machine";

  focusSystem.addComponent(pComponent_t(focus));

  CEventHandler* events = new CEventHandler(m_entityId);
  events->broadcastedEventHandlers.push_back(EventHandler{"key_pressed",
    [this](const GameEvent& e_) {

    auto& e = dynamic_cast<const EKeyPressed&>(e_);
    if (e.key == Qt::Key_C) {
      useCovfefe();
    }
  }});
  events->broadcastedEventHandlers.push_back(EventHandler{"t_minus_two_minutes/machine_jammed",
    [this, &focusSystem](const GameEvent&) {

    DBG_PRINT("Machine jammed\n");

    m_entityManager.deleteEntity(Component::getIdFromString("covfefe"));
    focusSystem.showCaption(m_entityId);
  }});

  eventHandlerSystem.addComponent(pComponent_t(events));
}

//===========================================
// GameLogic::useCovfefe
//===========================================
void GameLogic::useCovfefe() {
  entityId_t covfefeId = Component::getIdFromString("covfefe");

  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  auto& inventorySystem = m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);

  const Player& player = *spatialSystem.sg.player;
  auto& playerBody = dynamic_cast<const CVRect&>(spatialSystem.getComponent(player.body));

  const map<string, entityId_t>& items = inventorySystem.getBucketItems(player.body, "item");

  if (contains(items, string{"covfefe"})) {
    DBG_PRINT("Using covfefe\n");
    m_audioService.playSound("covfefe");

    inventorySystem.removeFromBucket(player.body, "item", "covfefe");

    double distance = 100.0;
    double margin = 10.0;
    Point pos = playerBody.pos + player.dir() * distance;

    vector<pIntersection_t> intersections = spatialSystem.entitiesAlongRay(Vec2f{1, 0});

    if (intersections.front()->distanceFromOrigin - margin < distance) {
      pos = intersections.front()->point_wld - player.dir() * margin;
    }

    spatialSystem.relocateEntity(covfefeId, *playerBody.zone, pos);

    double heightAboveFloor = 50.0;

    set<entityId_t> entities = spatialSystem.entitiesInRadius(*playerBody.zone, pos, 50.0,
      heightAboveFloor);
    m_entityManager.fireEvent(GameEvent{"t_minus_two_minutes/covfefe_impact"}, entities);
  }
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {

}


}
