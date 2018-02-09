#include "raycast/misc_factory.hpp"
#include "raycast/root_factory.hpp"
#include "raycast/geometry.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/behaviour_system.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/render_system.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/behaviour_system.hpp"
#include "raycast/c_enemy_behaviour.hpp"
#include "raycast/c_door_behaviour.hpp"
#include "raycast/c_switch_behaviour.hpp"
#include "raycast/audio_service.hpp"
#include "raycast/time_service.hpp"
#include "exception.hpp"


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
  static const set<string> types{"player_inventory", "door", "switch", "elevator"};
  return types;
}

//===========================================
// MiscFactory::constructPlayerInventory
//===========================================
bool MiscFactory::constructPlayerInventory() {
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

  return true;
}

//===========================================
// MiscFactory::constructDoor
//===========================================
bool MiscFactory::constructDoor(entityId_t entityId, const parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform)) {
    BehaviourSystem& behaviourSystem =
      m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

    CDoorBehaviour* behaviour = new CDoorBehaviour(entityId, m_entityManager,
      m_timeService.frameRate);

    behaviourSystem.addComponent(pComponent_t(behaviour));

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructSwitch
//===========================================
bool MiscFactory::constructSwitch(entityId_t entityId, const parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("wall_decal", entityId, obj, parentId, parentTransform)) {
    BehaviourSystem& behaviourSystem =
      m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

    entityId_t target = Component::getIdFromString(getValue(obj.dict, "target"));
    bool toggleable = getValue(obj.dict, "toggleable", "false") == "true";
    double toggleDelay = std::stod(getValue(obj.dict, "toggle_delay", "0.0"));

    SwitchState initialState = SwitchState::OFF;
    if (getValue(obj.dict, "initial_state", "") == "on") {
      initialState = SwitchState::ON;
    }

    CSwitchBehaviour* behaviour = new CSwitchBehaviour(entityId, m_entityManager, target,
      initialState, toggleable, toggleDelay);

    behaviourSystem.addComponent(pComponent_t(behaviour));

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructElevator
//===========================================
bool MiscFactory::constructElevator(entityId_t entityId, const parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform)) {
    SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
    CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(entityId));

    double targetH = std::stod(getValue(obj.dict, "target_floor_height"));
    double speed = 60.0;
    double dy = speed / m_timeService.frameRate;

    EventHandlerSystem& eventHandlerSystem
      = m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

    CEventHandler* handler = new CEventHandler(entityId);
    handler->handlers.push_back(EventHandler{"switchActivateEntity",
      [=, &zone](const GameEvent& e) {

      DBG_PRINT("Elevator activated\n");

      if (fabs(targetH - zone.floorHeight) >= dy) {
        m_timeService.addTween(Tween{[=, &zone](long, double, double) {
          zone.floorHeight += dy;
          return fabs(targetH - zone.floorHeight) >= dy;
        }, [](long, double, double) {}});
      }
    }});

    eventHandlerSystem.addComponent(pComponent_t(handler));

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructObject
//===========================================
bool MiscFactory::constructObject(const string& type, entityId_t entityId,
  const parser::Object& obj, entityId_t parentId, const Matrix& parentTransform) {

  if (type == "player_inventory") {
    return constructPlayerInventory();
  }
  else if (type == "door") {
    return constructDoor(entityId, obj, parentId, parentTransform);
  }
  else if (type == "switch") {
    return constructSwitch(entityId, obj, parentId, parentTransform);
  }
  else if (type == "elevator") {
    return constructElevator(entityId, obj, parentId, parentTransform);
  }

  return false;
}
