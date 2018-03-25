#include <algorithm>
#include <vector>
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
#include "raycast/spawn_system.hpp"
#include "raycast/c_door_behaviour.hpp"
#include "raycast/c_elevator_behaviour.hpp"
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
using std::vector;


//===========================================
// setBoolean
//===========================================
static void setBoolean(bool& b, const std::string& s) {
  if (s == "true") {
    b = true;
  }
  else if (s == "false") {
    b = false;
  }
}

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
  static const set<string> types{"player_inventory", "door", "switch", "elevator", "spawn_point"};
  return types;
}

//===========================================
// MiscFactory::constructPlayerInventory
//===========================================
bool MiscFactory::constructPlayerInventory() {
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  InventorySystem& inventorySystem =
    m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);
  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
  DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);

  const Player& player = *spatialSystem.sg.player;
  const Size& viewport = renderSystem.rg.viewport;

  entityId_t ammoId = Component::getNextId();
  entityId_t healthId = Component::getNextId();

  CBucket* ammoBucket = new CBucket(ammoId, "ammo", 50);
  inventorySystem.addComponent(pComponent_t(ammoBucket));

  CTextOverlay* ammoCounter = new CTextOverlay(ammoId, "AMMO 0/50", Point(0.1, viewport.y - 0.5),
    0.5, Qt::green, 2);
  renderSystem.addComponent(pComponent_t(ammoCounter));

  CTextOverlay* healthCounter = new CTextOverlay(healthId, "HEALTH 10/10",
    Point(9.5, viewport.y - 0.5), 0.5, Qt::red, 2);
  renderSystem.addComponent(pComponent_t(healthCounter));

  CEventHandler* syncCounters = new CEventHandler(ammoId);
  syncCounters->handlers.push_back(EventHandler{"bucket_count_change", [=](const GameEvent& e_) {
    const EBucketCountChange& e = dynamic_cast<const EBucketCountChange&>(e_);

    stringstream ss;
    ss << "AMMO " << ammoBucket->count << "/" << ammoBucket->capacity;
    ammoCounter->text = ss.str();

    if (e.currentCount > e.prevCount) {
      m_audioService.playSound("ammo_collect");
    }
  }});
  syncCounters->handlers.push_back(EventHandler{"entity_damaged",
    [=, &damageSystem, &player](const GameEvent&) {

    const CDamage& damage = dynamic_cast<const CDamage&>(damageSystem.getComponent(player.body));

    stringstream ss;
    ss << "HEALTH " << damage.health << "/" << damage.maxHealth;
    healthCounter->text = ss.str();
  }});

  eventHandlerSystem.addComponent(pComponent_t(syncCounters));

  entityId_t bgId = Component::getNextId();

  CColourOverlay* bg = new CColourOverlay(bgId, QColor(0, 0, 0, 100), Point(0, viewport.y - 0.7),
    Size(viewport.x, 0.7), 1);

  renderSystem.addComponent(pComponent_t(bg));

  return true;
}

//===========================================
// MiscFactory::constructSpawnPoint
//===========================================
bool MiscFactory::constructSpawnPoint(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  SpawnSystem& spawnSystem = m_entityManager.system<SpawnSystem>(ComponentKind::C_SPAWN);

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(parentId));

  CVRect* vRect = new CVRect(entityId, zone.entityId(), Size(1, 1));
  Matrix m = transformFromTriangle(obj.path);
  vRect->setTransform(parentTransform * obj.transform * m);
  vRect->zone = &zone;

  spatialSystem.addComponent(pComponent_t(vRect));

  CSpawnPoint* spawnPoint = new CSpawnPoint(entityId);
  spawnSystem.addComponent(pComponent_t(spawnPoint));

  return true;
}

//===========================================
// MiscFactory::constructDoor
//===========================================
bool MiscFactory::constructDoor(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform)) {
    BehaviourSystem& behaviourSystem =
      m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

    CDoorBehaviour* behaviour = new CDoorBehaviour(entityId, m_entityManager,
      m_timeService.frameRate);

    string s = getValue(obj.dict, "player_activated", "");
    setBoolean(behaviour->isPlayerActivated, s);

    s = getValue(obj.dict, "player_activated", "");
    setBoolean(behaviour->closeAutomatically, s);

    behaviour->openOnEvent = getValue(obj.dict, "open_on_event", "");

    behaviourSystem.addComponent(pComponent_t(behaviour));

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructSwitch
//===========================================
bool MiscFactory::constructSwitch(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

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

    string message = getValue(obj.dict, "message", "");

    CSwitchBehaviour* behaviour = new CSwitchBehaviour(entityId, m_entityManager, target, message,
      initialState, toggleable, toggleDelay);

    string requiredItem = getValue(obj.dict, "required_item", "");
    if (requiredItem != "") {
      behaviour->requiredItem = requiredItem;
    }

    behaviourSystem.addComponent(pComponent_t(behaviour));

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructElevator
//===========================================
bool MiscFactory::constructElevator(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform)) {
    BehaviourSystem& behaviourSystem =
      m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

    vector<string> strLevels = splitString(getValue(obj.dict, "levels"), ',');
    vector<double> levels(strLevels.size());
    std::transform(strLevels.begin(), strLevels.end(), levels.begin(), [](const string& s) {
      return std::stod(s);
    });

    CElevatorBehaviour* behaviour = new CElevatorBehaviour(entityId, m_entityManager,
      m_timeService.frameRate, levels);

    if (contains<string>(obj.dict, "speed")) {
      double speed = std::stod(obj.dict.at("speed"));
      behaviour->setSpeed(speed);
    }

    behaviourSystem.addComponent(pComponent_t(behaviour));

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructObject
//===========================================
bool MiscFactory::constructObject(const string& type, entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

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
  else if (type == "spawn_point") {
    return constructSpawnPoint(entityId, obj, parentId, parentTransform);
  }

  return false;
}
