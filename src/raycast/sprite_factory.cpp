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

  CZone& zone = spatialSys().zone(parentId);

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  string texName = getValue(obj.dict, "texture", "default");
  const Texture& texture = GET_VALUE(renderSys().rg.textures, texName);

  double height = std::stod(getValue(obj.dict, "height", "0.0"));

  CVRect* vRect = new CVRect(entityId, zone.entityId(), texture.size_wd);
  Matrix m = transformFromTriangle(obj.path);
  vRect->setTransform(parentTransform * obj.groupTransform * obj.pathTransform * m);
  vRect->zone = &zone;
  vRect->y = height;

  spatialSys().addComponent(pComponent_t(vRect));

  CSprite* sprite = new CSprite(entityId, zone.entityId(), texName);
  sprite->texViews = {
    QRectF(0, 0, 1, 1)
  };

  renderSys().addComponent(pComponent_t(sprite));

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
    CCollectable* collectable = new CCollectable(entityId, "ammo");
    collectable->value = 5;
    inventorySys().addComponent(pComponent_t(collectable));

    return true;
  }

  return false;
}

//===========================================
// SpriteFactory::constructHealthPack
//===========================================
bool SpriteFactory::constructHealthPack(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["texture"] = "health_pack";

  if (m_rootFactory.constructObject("sprite", entityId, obj, parentId, parentTransform)) {
    // Health packs don't have a bucket, but the Player class listens for their collection and
    // removes them accordingly

    CCollectable* collectable = new CCollectable(entityId, "health_pack");
    collectable->value = 1;
    inventorySys().addComponent(pComponent_t(collectable));

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
    CCollector* inventory = new CCollector(entityId);
    inventory->buckets["item"] = pBucket_t(new ItemBucket(1));
    inventorySys().addComponent(pComponent_t(inventory));

    string name = getValue(obj.dict, "name", "");
    if (name != "") {
      CFocus* focus = new CFocus(entityId);
      focus->hoverText = name.replace(0, 1, 1, asciiToUpper(name[0]));
      focusSys().addComponent(pComponent_t(focus));
    }

    // Number of frames in sprite sheet
    const int W = 8;
    const int H = 14;

    CAnimation* anim = new CAnimation(entityId);

    vector<AnimationFrame> frames = constructFrames(W, H,
      { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 });
    anim->addAnimation(pAnimation_t(new Animation("run", m_timeService.frameRate, 1.0, frames)));

    frames = constructFrames(W, H, { 0 });
    anim->addAnimation(pAnimation_t(new Animation("idle", m_timeService.frameRate, 1.0, frames)));

    frames = {{
      AnimationFrame{{QRectF{0.0 / W, 13.0 / H, 1.0 / W, 1.0 / H}}},
      AnimationFrame{{QRectF{1.0 / W, 13.0 / H, 1.0 / W, 1.0 / H}}},
      AnimationFrame{{QRectF{2.0 / W, 13.0 / H, 1.0 / W, 1.0 / H}}},
      AnimationFrame{{QRectF{3.0 / W, 13.0 / H, 1.0 / W, 1.0 / H}}},
    }};
    anim->addAnimation(pAnimation_t(new Animation("death", m_timeService.frameRate, 0.5, frames)));

    animationSys().addComponent(pComponent_t(anim));
    animationSys().playAnimation(entityId, "idle", true);

    CDamage* damage = new CDamage(entityId, 2, 2);
    damageSys().addComponent(pComponent_t(damage));

    CVRect& vRect = m_entityManager.getComponent<CVRect>(entityId, ComponentKind::C_SPATIAL);

    CEventHandler* events = new CEventHandler(entityId);
    events->targetedEventHandlers.push_back(EventHandler{"entity_damaged",
      [=, &vRect](const GameEvent&) {

      DBG_PRINT("Civilian health: " << damage->health << "\n");
      //animationSys().playAnimation(entityId, "hurt", false);

      if (damage->health == 0) {
        m_audioService.playSoundAtPos("civilian_death", vRect.pos);
        auto& bucket = dynamic_cast<ItemBucket&>(*inventory->buckets["item"]);

        for (auto it = bucket.items.begin(); it != bucket.items.end(); ++it) {
          entityId_t itemId = it->second;
          const CVRect& body = dynamic_cast<const CVRect&>(spatialSys().getComponent(entityId));
          spatialSys().relocateEntity(itemId, *body.zone, body.pos);
        }

        agentSys().removeEntity(entityId);
        animationSys().playAnimation(entityId, "death", false);
      }
      else {
        m_audioService.playSoundAtPos("civilian_hurt", vRect.pos);
      }
    }});
    events->targetedEventHandlers.push_back(EventHandler{"animation_finished",
      [this, entityId](const GameEvent& e_) {

      auto& e = dynamic_cast<const EAnimationFinished&>(e_);
      if (e.animName == "death") {
        m_entityManager.deleteEntity(entityId);
      }
    }});

    eventHandlerSys().addComponent(pComponent_t(events));

    CAgent* agent = new CAgent(entityId, m_entityManager);
    agent->stPatrollingTrigger = getValue(obj.dict, "st_patrolling_trigger", "");
    agent->isHostile = false;

    string s = getValue(obj.dict, "patrol_path", "");
    if (s != "") {
      agent->patrolPath = Component::getIdFromString(s);
    }

    agentSys().addComponent(pComponent_t(agent));

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      parser::Object& ch = **it;
      entityId_t invItemId = makeIdForObj(ch);

      if (m_rootFactory.constructObject(ch.type, invItemId, ch, entityId,
        parentTransform * obj.groupTransform)) {

        const CCollectable& item =
          dynamic_cast<const CCollectable&>(inventorySys().getComponent(invItemId));

        inventorySys().addToBucket(entityId, item);
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
    // Number of frames in sprite sheet
    const int W = 8;
    const int H = 15;

    CAnimation* anim = new CAnimation(entityId);

    vector<AnimationFrame> frames = constructFrames(W, H,
      { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 });
    anim->addAnimation(pAnimation_t(new Animation("run", m_timeService.frameRate, 1.0, frames)));

    frames = constructFrames(W, H, { 1, 0 });
    anim->addAnimation(pAnimation_t(new Animation("shoot", m_timeService.frameRate, 1.0, frames)));

    frames = constructFrames(W, H, { 0 });
    anim->addAnimation(pAnimation_t(new Animation("idle", m_timeService.frameRate, 1.0, frames)));

    frames = {{
      AnimationFrame{{QRectF{0.0 / W, 14.0 / H, 1.0 / W, 1.0 / H}}},
      AnimationFrame{{QRectF{1.0 / W, 14.0 / H, 1.0 / W, 1.0 / H}}},
      AnimationFrame{{QRectF{2.0 / W, 14.0 / H, 1.0 / W, 1.0 / H}}},
      AnimationFrame{{QRectF{3.0 / W, 14.0 / H, 1.0 / W, 1.0 / H}}},
    }};
    anim->addAnimation(pAnimation_t(new Animation("death", m_timeService.frameRate, 0.5, frames)));

    animationSys().addComponent(pComponent_t(anim));
    animationSys().playAnimation(entityId, "idle", true);

    string s = getValue(obj.dict, "spawn_point", "");
    if (s != "") {
      CSpawnable* spawnable = new CSpawnable(entityId, Component::getIdFromString(s),
        "bad_guy", parser::pObject_t(obj.clone()), parentId, parentTransform);

      string delay = getValue(obj.dict, "spawn_delay", "");
      if (delay != "") {
        spawnable->delay = std::stod(delay);
      }

      spawnSys().addComponent(pComponent_t(spawnable));
    }

    CDamage* damage = new CDamage(entityId, 2, 2);
    damageSys().addComponent(pComponent_t(damage));

    CVRect& vRect = m_entityManager.getComponent<CVRect>(entityId, ComponentKind::C_SPATIAL);

    CEventHandler* events = new CEventHandler(entityId);
    events->targetedEventHandlers.push_back(EventHandler{"entity_damaged",
      [=, &vRect](const GameEvent& e) {

      const EEntityDamaged& event = dynamic_cast<const EEntityDamaged&>(e);

      if (event.entityId == entityId) {
        DBG_PRINT("Enemy health: " << damage->health << "\n");
        //animationSys().playAnimation(entityId, "hurt", false);

        if (damage->health == 0) {
          m_audioService.playSoundAtPos("monster_death", vRect.pos);
          agentSys().removeEntity(entityId);
          animationSys().playAnimation(entityId, "death", false);
        }
        else {
          m_audioService.playSoundAtPos("monster_hurt", vRect.pos);
        }
      }
    }});
    events->targetedEventHandlers.push_back(EventHandler{"animation_finished",
      [this, entityId](const GameEvent& e_) {

      auto& e = dynamic_cast<const EAnimationFinished&>(e_);
      if (e.animName == "death") {
        m_entityManager.deleteEntity(entityId);
      }
    }});
    eventHandlerSys().addComponent(pComponent_t(events));

    CAgent* agent = new CAgent(entityId, m_entityManager);

    agent->isHostile = true;
    agent->stPatrollingTrigger = getValue(obj.dict, "st_patrolling_trigger", "");
    agent->stChasingTrigger = getValue(obj.dict, "st_chasing_trigger", "");

    s = getValue(obj.dict, "patrol_path", "");
    if (s != "") {
      agent->patrolPath = Component::getIdFromString(s);
    }

    agentSys().addComponent(pComponent_t(agent));

    return true;
  }

  return false;
}

//===========================================
// SpriteFactory::SpriteFactory
//===========================================
SpriteFactory::SpriteFactory(RootFactory& rootFactory, EntityManager& entityManager,
  AudioService& audioService, TimeService& timeService)
  : SystemAccessor(entityManager),
    m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_audioService(audioService),
    m_timeService(timeService) {}

//===========================================
// SpriteFactory::types
//===========================================
const set<string>& SpriteFactory::types() const {
  static const set<string> types{
    "sprite",
    "bad_guy",
    "civilian",
    "ammo",
    "health_pack"};

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
  else if (type == "health_pack") {
    return constructHealthPack(entityId, obj, region, parentTransform);
  }

  return false;
}
