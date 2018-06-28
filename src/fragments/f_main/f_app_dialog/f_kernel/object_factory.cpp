#include "fragments/f_main/f_app_dialog/f_kernel/object_factory.hpp"
#include "fragments/f_main/f_app_dialog/f_kernel/game_events.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/behaviour_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/c_door_behaviour.hpp"
#include "raycast/root_factory.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/time_service.hpp"
#include "raycast/audio_service.hpp"


using std::vector;
using std::set;
using std::string;


namespace doomsweeper {


//===========================================
// ObjectFactory::ObjectFactory
//===========================================
ObjectFactory::ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager,
  TimeService& timeService, AudioService& audioService)
  : m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_timeService(timeService),
    m_audioService(audioService) {}

//===========================================
// ObjectFactory::types
//===========================================
const set<string>& ObjectFactory::types() const {
  static const set<string> types{
    "cell",
    "cell_inner",
    "cell_corner",
    "cell_door",
    "slime"};

  return types;
}

//===========================================
// ObjectFactory::constructCell
//===========================================
bool ObjectFactory::constructCell(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (!firstPassComplete) {
    region = parentId;
    this->parentTransform = parentTransform;
    this->objects.insert(make_pair(entityId, parser::pObject_t(obj.clone())));
  }
  else {
    return m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform);
  }

  return true;
}

//===========================================
// ObjectFactory::constructCellCorner
//===========================================
bool ObjectFactory::constructCellCorner(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("v_rect", entityId, obj, parentId, parentTransform)) {
    auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
    const CVRect& vRect = dynamic_cast<const CVRect&>(spatialSystem.getComponent(entityId));

    string cellName = getValue(obj.dict, "cell_name");
    entityId_t cellId = Component::getIdFromString(cellName);

    DBG_PRINT("Cell '" << cellName << "'" << " is positioned at " << vRect.pos << "\n");
    this->objectPositions[cellId] = vRect.pos;

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructCellInner
//===========================================
bool ObjectFactory::constructCellInner(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  string type = "region";

  if (getValue(obj.dict, "is_slime_pit", "false") == "true") {
    type = "slime";
  }

  if (m_rootFactory.constructObject(type, entityId, obj, parentId, parentTransform)) {
    auto& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

    CEventHandler* events = new CEventHandler(entityId);
    events->broadcastedEventHandlers.push_back(EventHandler{"entity_changed_zone",
      [this, entityId, parentId](const GameEvent& e_) {

      const auto& e = dynamic_cast<const EChangedZone&>(e_);

      if (e.newZone == entityId) {
        m_entityManager.broadcastEvent(EPlayerEnterCellInner{parentId});
      }
    }});

    eventHandlerSystem.addComponent(pComponent_t(events));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructCellDoor
//===========================================
bool ObjectFactory::constructCellDoor(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["denied_caption"] = "The door is locked";

  if (m_rootFactory.constructObject("door", entityId, obj, parentId, parentTransform)) {
    auto& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

    auto& behaviour =
      m_entityManager.getComponent<CDoorBehaviour>(entityId, ComponentKind::C_BEHAVIOUR);

    behaviour.speed = 120.0;
    behaviour.setPauseTime(1.5);

    // Cell should be 2 levels up
    entityId_t cellId = Component::getIdFromString(obj.parent->parent->dict.at("name"));

    string pos = GET_VALUE(obj.dict, "position");
    if (pos == "north") {
      this->cellDoors[cellId].north = entityId;
    }
    else if (pos == "east") {
      this->cellDoors[cellId].east = entityId;
    }
    else if (pos == "south") {
      this->cellDoors[cellId].south = entityId;
    }
    else if (pos == "west") {
      this->cellDoors[cellId].west = entityId;
    }

    CEventHandler* events = new CEventHandler(entityId);
    events->targetedEventHandlers.push_back(EventHandler{"door_open_start",
      [this, cellId](const GameEvent& e_) {

      m_entityManager.broadcastEvent(ECellDoorOpened{cellId});
    }});

    eventHandlerSystem.addComponent(pComponent_t(events));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructSlime
//===========================================
bool ObjectFactory::constructSlime(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["floor_texture"] = "slime";

  if (m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform)) {
    auto& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
    auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
    const Player& player = *spatialSystem.sg.player;
    auto& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
    auto& animationSystem = m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);

    CAnimation* anim = new CAnimation(entityId);
    vector<AnimationFrame> frames = constructFrames(1, 3, { 0, 1, 2 });
    anim->addAnimation(pAnimation_t(new Animation("gurgle", m_timeService.frameRate, 1.0, frames)));

    animationSystem.addComponent(pComponent_t(anim));

    animationSystem.playAnimation(entityId, "gurgle", true);

    CEventHandler* events = new CEventHandler(entityId);
    events->broadcastedEventHandlers.push_back(EventHandler{"entity_changed_zone",
      [=, &damageSystem, &player](const GameEvent& e_) mutable {

      auto& e = dynamic_cast<const EChangedZone&>(e_);

      if (e.newZone == entityId) {
        m_timeService.atIntervals([=, &player, &damageSystem]() {
          if (!player.aboveGround()) {
            damageSystem.damageEntity(player.body, 1);
          }
          return player.region() == entityId;
        }, 0.5);
      }
    }});

    eventHandlerSystem.addComponent(pComponent_t(events));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructObject
//===========================================
bool ObjectFactory::constructObject(const string& type, entityId_t entityId,
  parser::Object& obj, entityId_t region, const Matrix& parentTransform) {

  if (type == "cell") {
    return constructCell(entityId, obj, region, parentTransform);
  }
  else if (type == "cell_inner") {
    return constructCellInner(entityId, obj, region, parentTransform);
  }
  else if (type == "cell_door") {
    return constructCellDoor(entityId, obj, region, parentTransform);
  }
  else if (type == "cell_corner") {
    return constructCellCorner(entityId, obj, region, parentTransform);
  }
  else if (type == "slime") {
    return constructSlime(entityId, obj, region, parentTransform);
  }

  return false;
}


}
