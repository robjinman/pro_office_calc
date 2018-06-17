#include "raycast/sprite_factory.hpp"
#include "raycast/geometry.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/render_system.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/spawn_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/agent_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/audio_service.hpp"
#include "raycast/time_service.hpp"
#include "raycast/root_factory.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::vector;
using std::string;
using std::set;


//===========================================
// SpriteFactory::constructSprite
//===========================================
bool SpriteFactory::constructSprite(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  CZone& zone = spatialSystem.zone(parentId);

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  string texName = getValue(obj.dict, "texture", "default");
  const Texture& texture = GET_VALUE(renderSystem.rg.textures, texName);

  double height = std::stod(getValue(obj.dict, "height", "0.0"));

  CVRect* vRect = new CVRect(entityId, zone.entityId(), texture.size_wd);
  Matrix m = transformFromTriangle(obj.path);
  vRect->setTransform(parentTransform * obj.groupTransform * obj.pathTransform * m);
  vRect->zone = &zone;
  vRect->y = height;

  spatialSystem.addComponent(pComponent_t(vRect));

  CSprite* sprite = new CSprite(entityId, zone.entityId(), texName);
  sprite->texViews = {
    QRectF(0, 0, 1, 1)
  };

  renderSystem.addComponent(pComponent_t(sprite));

  return true;
}

//===========================================
// SpriteFactory::constructAmmo
//===========================================
bool SpriteFactory::constructAmmo(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["texture"] = "ammo";

  if (m_rootFactory.constructObject("sprite", entityId, obj, parentId, parentTransform)) {
    InventorySystem& inventorySystem =
      m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);

    CCollectable* collectable = new CCollectable(entityId, "ammo");
    collectable->value = 5;
    inventorySystem.addComponent(pComponent_t(collectable));

    return true;
  }

  return false;
}

//===========================================
// SpriteFactory::constructCivilian
//===========================================
bool SpriteFactory::constructCivilian(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (!contains<string>(obj.dict, "texture")) {
    obj.dict["texture"] = "civilian";
  }

  if (m_rootFactory.constructObject("sprite", entityId, obj, parentId, parentTransform)) {
    AnimationSystem& animationSystem =
      m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);
    DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
    EventHandlerSystem& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
    AgentSystem& agentSystem = m_entityManager.system<AgentSystem>(ComponentKind::C_AGENT);
    InventorySystem& inventorySystem =
      m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);
    SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
    FocusSystem& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);

    CCollector* inventory = new CCollector(entityId);
    inventory->buckets["item"] = pBucket_t(new ItemBucket(1));
    inventorySystem.addComponent(pComponent_t(inventory));

    string name = getValue(obj.dict, "name", "");
    if (name != "") {
      CFocus* focus = new CFocus(entityId);
      focus->hoverText = name.replace(0, 1, 1, asciiToUpper(name[0]));
      focusSystem.addComponent(pComponent_t(focus));
    }

    // Number of frames in sprite sheet
    const int W = 8;
    const int H = 13;

    CAnimation* anim = new CAnimation(entityId);

    vector<AnimationFrame> frames = constructFrames(W, H,
      { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 });
    anim->addAnimation(pAnimation_t(new Animation("run", m_timeService.frameRate, 1.0, frames)));

    frames = constructFrames(W, H, { 0 });
    anim->addAnimation(pAnimation_t(new Animation("idle", m_timeService.frameRate, 1.0, frames)));

    animationSystem.addComponent(pComponent_t(anim));
    animationSystem.playAnimation(entityId, "idle", true);

    CDamage* damage = new CDamage(entityId, 2, 2);
    damageSystem.addComponent(pComponent_t(damage));

    CVRect& vRect = m_entityManager.getComponent<CVRect>(entityId, ComponentKind::C_SPATIAL);

    CEventHandler* takeDamage = new CEventHandler(entityId);
    takeDamage->targetedEventHandlers.push_back(EventHandler{"entity_damaged",
      [=, &animationSystem, &spatialSystem, &vRect](const GameEvent&) {

      DBG_PRINT("Civilian health: " << damage->health << "\n");
      //animationSystem.playAnimation(entityId, "hurt", false);

      if (damage->health == 0) {
        m_audioService.playSoundAtPos("civilian_death", vRect.pos);
        auto& bucket = dynamic_cast<ItemBucket&>(*inventory->buckets["item"]);

        for (auto it = bucket.items.begin(); it != bucket.items.end(); ++it) {
          entityId_t itemId = it->second;
          const CVRect& body = dynamic_cast<const CVRect&>(spatialSystem.getComponent(entityId));
          spatialSystem.relocateEntity(itemId, *body.zone, body.pos);
        }

        m_entityManager.deleteEntity(entityId);
      }
      else {
        m_audioService.playSoundAtPos("civilian_hurt", vRect.pos);
      }
    }});
    eventHandlerSystem.addComponent(pComponent_t(takeDamage));

    CAgent* agent = new CAgent(entityId);
    agent->stPatrollingTrigger = getValue(obj.dict, "st_patrolling_trigger", "");
    agent->isHostile = false;

    string s = getValue(obj.dict, "patrol_path", "");
    if (s != "") {
      agent->patrolPath = Component::getIdFromString(s);
    }

    agentSystem.addComponent(pComponent_t(agent));

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      parser::Object& ch = **it;
      entityId_t invItemId = makeIdForObj(ch);

      if (m_rootFactory.constructObject(ch.type, invItemId, ch, entityId,
        parentTransform * obj.groupTransform)) {

        const CCollectable& item =
          dynamic_cast<const CCollectable&>(inventorySystem.getComponent(invItemId));

        inventorySystem.addToBucket(entityId, item);
      }
    }

    return true;
  }

  return false;
}

//===========================================
// SpriteFactory::constructBadGuy
//===========================================
bool SpriteFactory::constructBadGuy(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["texture"] = "bad_guy";

  if (m_rootFactory.constructObject("sprite", entityId, obj, parentId, parentTransform)) {
    AnimationSystem& animationSystem =
      m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);
    DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
    EventHandlerSystem& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
    SpawnSystem& spawnSystem = m_entityManager.system<SpawnSystem>(ComponentKind::C_SPAWN);
    AgentSystem& agentSystem = m_entityManager.system<AgentSystem>(ComponentKind::C_AGENT);

    // Number of frames in sprite sheet
    const int W = 8;
    const int H = 14;

    CAnimation* anim = new CAnimation(entityId);

    vector<AnimationFrame> frames = constructFrames(W, H,
      { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 });
    anim->addAnimation(pAnimation_t(new Animation("run", m_timeService.frameRate, 1.0, frames)));

    frames = constructFrames(W, H, { 1, 0 });
    anim->addAnimation(pAnimation_t(new Animation("shoot", m_timeService.frameRate, 1.0, frames)));

    frames = constructFrames(W, H, { 0 });
    anim->addAnimation(pAnimation_t(new Animation("idle", m_timeService.frameRate, 1.0, frames)));

    animationSystem.addComponent(pComponent_t(anim));
    animationSystem.playAnimation(entityId, "idle", true);

    string s = getValue(obj.dict, "spawn_point", "");
    if (s != "") {
      CSpawnable* spawnable = new CSpawnable(entityId, Component::getIdFromString(s),
        "bad_guy", parser::pObject_t(obj.clone()), parentId, parentTransform);

      string delay = getValue(obj.dict, "spawn_delay", "");
      if (delay != "") {
        spawnable->delay = std::stod(delay);
      }

      spawnSystem.addComponent(pComponent_t(spawnable));
    }

    CDamage* damage = new CDamage(entityId, 2, 2);
    damageSystem.addComponent(pComponent_t(damage));

    CVRect& vRect = m_entityManager.getComponent<CVRect>(entityId, ComponentKind::C_SPATIAL);

    CEventHandler* takeDamage = new CEventHandler(entityId);
    takeDamage->targetedEventHandlers.push_back(EventHandler{"entity_damaged",
      [=, &animationSystem, &vRect](const GameEvent& e) {

      const EEntityDamaged& event = dynamic_cast<const EEntityDamaged&>(e);

      if (event.entityId == entityId) {
        DBG_PRINT("Enemy health: " << damage->health << "\n");
        animationSystem.playAnimation(entityId, "hurt", false);

        if (damage->health == 0) {
          m_audioService.playSoundAtPos("monster_death", vRect.pos);
          m_entityManager.deleteEntity(entityId);
        }
        else {
          m_audioService.playSoundAtPos("monster_hurt", vRect.pos);
        }
      }
    }});
    eventHandlerSystem.addComponent(pComponent_t(takeDamage));

    CAgent* agent = new CAgent(entityId);

    agent->isHostile = true;
    agent->stPatrollingTrigger = getValue(obj.dict, "st_patrolling_trigger", "");
    agent->stChasingTrigger = getValue(obj.dict, "st_chasing_trigger", "");

    s = getValue(obj.dict, "patrol_path", "");
    if (s != "") {
      agent->patrolPath = Component::getIdFromString(s);
    }

    agentSystem.addComponent(pComponent_t(agent));

    return true;
  }

  return false;
}

//===========================================
// SpriteFactory::SpriteFactory
//===========================================
SpriteFactory::SpriteFactory(RootFactory& rootFactory, EntityManager& entityManager,
  AudioService& audioService, TimeService& timeService)
  : m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_audioService(audioService),
    m_timeService(timeService) {}

//===========================================
// SpriteFactory::types
//===========================================
const set<string>& SpriteFactory::types() const {
  static const set<string> types{"sprite", "bad_guy", "civilian", "ammo"};
  return types;
}

//===========================================
// SpriteFactory::constructObject
//===========================================
bool SpriteFactory::constructObject(const string& type, entityId_t entityId,
  parser::Object& obj, entityId_t region, const Matrix& parentTransform) {

  if (type == "sprite") {
    return constructSprite(entityId, obj, region, parentTransform);
  }
  else if (type == "bad_guy") {
    return constructBadGuy(entityId, obj, region, parentTransform);
  }
  else if (type == "civilian") {
    return constructCivilian(entityId, obj, region, parentTransform);
  }
  else if (type == "ammo") {
    return constructAmmo(entityId, obj, region, parentTransform);
  }

  return false;
}
