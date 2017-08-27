#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/misc_factory.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/root_factory.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/animation_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/inventory_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/damage_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/event_handler_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/c_enemy_behaviour.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/c_door_behaviour.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/audio_service.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/time_service.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::stringstream;
using std::unique_ptr;
using std::function;
using std::string;
using std::list;
using std::map;
using std::set;


//===========================================
// MiscFactory::MiscFactory
//===========================================
MiscFactory::MiscFactory(RootFactory& rootFactory, EntityManager& entityManager,
  AudioService& audioService, TimeService& timeService)
  : m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_audioService(audioService),
    m_timeService(timeService) {}

//===========================================
// MiscFactory::types
//===========================================
const set<string>& MiscFactory::types() const {
  static const set<string> types{"player_inventory", "door"};
  return types;
}

//===========================================
// MiscFactory::constructPlayerInventory
//===========================================
void MiscFactory::constructPlayerInventory() {
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  InventorySystem& inventorySystem =
    m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);
  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  entityId_t ammoId = Component::getNextId();

  CBucket* ammoBucket = new CBucket(ammoId, "ammo", 50);
  inventorySystem.addComponent(pComponent_t(ammoBucket));

  CTextOverlay* ammoCounter = new CTextOverlay(ammoId, "AMMO 0/50", Point(0.1, 0.1), 0.5,
    Qt::green);
  renderSystem.addComponent(pComponent_t(ammoCounter));

  CEventHandler* syncCounter = new CEventHandler(ammoId);
  syncCounter->handlers.push_back(EventHandler{"bucketCountChange", [=](const GameEvent& e_) {
    const EBucketCountChange& e = dynamic_cast<const EBucketCountChange&>(e_);

    stringstream ss;
    ss << "AMMO " << ammoBucket->count << "/" << ammoBucket->capacity;
    ammoCounter->text = ss.str();

    if (e.currentCount > e.prevCount) {
      m_audioService.playSound("ammo_collect");
    }
  }});

  eventHandlerSystem.addComponent(pComponent_t(syncCounter));
}

//===========================================
// MiscFactory::constructDoor
//===========================================
void MiscFactory::constructDoor(entityId_t entityId, const parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = Component::getNextId();
  }

  m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform);

  BehaviourSystem& behaviourSystem =
    m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

  CDoorBehaviour* behaviour = new CDoorBehaviour(entityId, m_entityManager,
    m_timeService.frameRate);

  behaviourSystem.addComponent(pComponent_t(behaviour));
}

//===========================================
// MiscFactory::constructObject
//===========================================
void MiscFactory::constructObject(const string& type, entityId_t entityId,
  const parser::Object& obj, entityId_t parentId, const Matrix& parentTransform) {

  if (type == "player_inventory") {
    constructPlayerInventory();
  }
  else if (type == "door") {
    constructDoor(entityId, obj, parentId, parentTransform);
  }
}
