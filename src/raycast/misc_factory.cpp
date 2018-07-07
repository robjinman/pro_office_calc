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
#include "raycast/spawn_system.hpp"
#include "raycast/focus_system.hpp"
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
  static const set<string> types{
    "player",
    "player_inventory",
    "door",
    "switch",
    "elevator",
    "spawn_point",
    "collectable_item",
    "computer_screen",
    "water"
  };
  return types;
}

//===========================================
// MiscFactory::constructCollectableItem
//===========================================
bool MiscFactory::constructCollectableItem(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("sprite", entityId, obj, parentId, parentTransform)) {
    InventorySystem& inventorySystem =
      m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);

    CCollectable* collectable = new CCollectable(entityId, "item");

    if (!contains<string>(obj.dict, "name")) {
      EXCEPTION("collectable_item is missing 'name' property");
    }

    collectable->name = obj.dict.at("name");

    inventorySystem.addComponent(pComponent_t(collectable));

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructPlayer
//===========================================
bool MiscFactory::constructPlayer(parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  Player* player = new Player(m_entityManager, m_audioService, m_timeService, obj, parentId,
    parentTransform);

  spatialSystem.sg.player.reset(player);

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
  vRect->setTransform(parentTransform * obj.groupTransform * obj.pathTransform * m);
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
    auto& behaviourSystem = m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);
    auto& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);

    string caption = getValue(obj.dict, "denied_caption", "");

    if (caption != "") {
      CFocus* focus = new CFocus(entityId);
      focus->captionText = caption;

      focusSystem.addComponent(pComponent_t(focus));
    }

    CDoorBehaviour* behaviour = new CDoorBehaviour(entityId, m_entityManager, m_timeService,
      m_audioService);

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
    FocusSystem& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);

    entityId_t target = Component::getIdFromString(getValue(obj.dict, "target"));
    bool toggleable = getValue(obj.dict, "toggleable", "false") == "true";
    double toggleDelay = std::stod(getValue(obj.dict, "toggle_delay", "0.0"));

    SwitchState initialState = SwitchState::OFF;
    if (getValue(obj.dict, "initial_state", "") == "on") {
      initialState = SwitchState::ON;
    }

    string message = getValue(obj.dict, "message", "");

    CSwitchBehaviour* behaviour = new CSwitchBehaviour(entityId, m_entityManager, m_timeService,
      target, message, initialState, toggleable, toggleDelay);

    behaviour->requiredItemType = getValue(obj.dict, "required_item_type", "");
    behaviour->requiredItemName = getValue(obj.dict, "required_item_name", "");

    behaviourSystem.addComponent(pComponent_t(behaviour));

    string captionText = getValue(obj.dict, "caption", "");
    if (captionText != "") {
      CFocus* focus = new CFocus(entityId);
      focus->captionText = captionText;

      focusSystem.addComponent(pComponent_t(focus));
    }

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructComputerScreen
//===========================================
bool MiscFactory::constructComputerScreen(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("join", entityId, obj, parentId, parentTransform)) {
    AnimationSystem& animationSystem =
      m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);

    // Number of frames in sprite sheet
    const int W = 1;
    const int H = 23;

    CAnimation* anim = new CAnimation(entityId);

    vector<AnimationFrame> frames = constructFrames(W, H,
      { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 });
    anim->addAnimation(pAnimation_t(new Animation("idle", m_timeService.frameRate, 3.0, frames)));

    animationSystem.addComponent(pComponent_t(anim));
    animationSystem.playAnimation(entityId, "idle", true);

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

    string strInitLevelIdx = getValue(obj.dict, "init_level_idx", "0");
    int initLevelIdx = std::stod(strInitLevelIdx);

    CElevatorBehaviour* behaviour = new CElevatorBehaviour(entityId, m_entityManager,
      m_audioService, m_timeService.frameRate, levels, initLevelIdx);

    string strPlayerActivated = getValue(obj.dict, "player_activated", "");
    if (strPlayerActivated != "") {
      behaviour->isPlayerActivated = strPlayerActivated == "true";
    }

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
// MiscFactory::constructWater
//===========================================
bool MiscFactory::constructWater(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["floor_texture"] = "water";

  if (m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform)) {
    auto& animationSystem = m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);

    CAnimation* anim = new CAnimation(entityId);
    vector<AnimationFrame> frames = constructFrames(1, 3, { 0, 1, 2 });
    anim->addAnimation(pAnimation_t(new Animation("gurgle", m_timeService.frameRate, 1.0, frames)));

    animationSystem.addComponent(pComponent_t(anim));

    animationSystem.playAnimation(entityId, "gurgle", true);

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructObject
//===========================================
bool MiscFactory::constructObject(const string& type, entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (type == "player") {
    return constructPlayer(obj, parentId, parentTransform);
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
  else if (type == "collectable_item") {
    return constructCollectableItem(entityId, obj, parentId, parentTransform);
  }
  else if (type == "computer_screen") {
    return constructComputerScreen(entityId, obj, parentId, parentTransform);
  }
  else if (type == "water") {
    return constructWater(entityId, obj, parentId, parentTransform);
  }

  return false;
}
