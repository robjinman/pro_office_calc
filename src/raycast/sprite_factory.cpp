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
#include "raycast/audio_service.hpp"
#include "raycast/time_service.hpp"
#include "raycast/root_factory.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::string;
using std::set;


//===========================================
// SpriteFactory::constructSprite
//===========================================
bool SpriteFactory::constructSprite(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(parentId));

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  string texName = getValue(obj.dict, "texture", "default");
  const Texture& texture = renderSystem.rg.textures.at(texName);

  CVRect* vRect = new CVRect(entityId, zone.entityId(), texture.size_wd);
  Matrix m = transformFromTriangle(obj.path);
  vRect->setTransform(parentTransform * obj.transform * m);
  vRect->zone = &zone;

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

    CCollectable* collectable = new CCollectable(entityId, "ammo", 5);
    inventorySystem.addComponent(pComponent_t(collectable));

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

    CSprite& sprite = m_entityManager.getComponent<CSprite>(entityId, ComponentKind::C_RENDER);
    sprite.texViews = {
      QRectF(0, 0, 0.125, 0.125),
      QRectF(0.125, 0, 0.125, 0.125),
      QRectF(0.25, 0, 0.125, 0.125),
      QRectF(0.375, 0, 0.125, 0.125),
      QRectF(0.5, 0, 0.125, 0.125),
      QRectF(0.625, 0, 0.125, 0.125),
      QRectF(0.750, 0, 0.125, 0.125),
      QRectF(0.875, 0, 0.125, 0.125)
    };

    CAnimation* anim = new CAnimation(entityId);
    anim->animations.insert(std::make_pair("idle", Animation(m_timeService.frameRate, 1.0, {
      AnimationFrame{{
        QRectF(0, 0, 0.125, 0.125),
        QRectF(0.125, 0, 0.125, 0.125),
        QRectF(0.25, 0, 0.125, 0.125),
        QRectF(0.375, 0, 0.125, 0.125),
        QRectF(0.5, 0, 0.125, 0.125),
        QRectF(0.625, 0, 0.125, 0.125),
        QRectF(0.750, 0, 0.125, 0.125),
        QRectF(0.875, 0, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.125, 0.125, 0.125),
        QRectF(0.125, 0.125, 0.125, 0.125),
        QRectF(0.25, 0.125, 0.125, 0.125),
        QRectF(0.375, 0.125, 0.125, 0.125),
        QRectF(0.5, 0.125, 0.125, 0.125),
        QRectF(0.625, 0.125, 0.125, 0.125),
        QRectF(0.750, 0.125, 0.125, 0.125),
        QRectF(0.875, 0.125, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.25, 0.125, 0.125),
        QRectF(0.125, 0.25, 0.125, 0.125),
        QRectF(0.25, 0.25, 0.125, 0.125),
        QRectF(0.375, 0.25, 0.125, 0.125),
        QRectF(0.5, 0.25, 0.125, 0.125),
        QRectF(0.625, 0.25, 0.125, 0.125),
        QRectF(0.750, 0.25, 0.125, 0.125),
        QRectF(0.875, 0.25, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.375, 0.125, 0.125),
        QRectF(0.125, 0.375, 0.125, 0.125),
        QRectF(0.25, 0.375, 0.125, 0.125),
        QRectF(0.375, 0.375, 0.125, 0.125),
        QRectF(0.5, 0.375, 0.125, 0.125),
        QRectF(0.625, 0.375, 0.125, 0.125),
        QRectF(0.750, 0.375, 0.125, 0.125),
        QRectF(0.875, 0.375, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.5, 0.125, 0.125),
        QRectF(0.125, 0.5, 0.125, 0.125),
        QRectF(0.25, 0.5, 0.125, 0.125),
        QRectF(0.375, 0.5, 0.125, 0.125),
        QRectF(0.5, 0.5, 0.125, 0.125),
        QRectF(0.625, 0.5, 0.125, 0.125),
        QRectF(0.750, 0.5, 0.125, 0.125),
        QRectF(0.875, 0.5, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.625, 0.125, 0.125),
        QRectF(0.125, 0.625, 0.125, 0.125),
        QRectF(0.25, 0.625, 0.125, 0.125),
        QRectF(0.375, 0.625, 0.125, 0.125),
        QRectF(0.5, 0.625, 0.125, 0.125),
        QRectF(0.625, 0.625, 0.125, 0.125),
        QRectF(0.750, 0.625, 0.125, 0.125),
        QRectF(0.875, 0.625, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.75, 0.125, 0.125),
        QRectF(0.125, 0.75, 0.125, 0.125),
        QRectF(0.25, 0.75, 0.125, 0.125),
        QRectF(0.375, 0.75, 0.125, 0.125),
        QRectF(0.5, 0.75, 0.125, 0.125),
        QRectF(0.625, 0.75, 0.125, 0.125),
        QRectF(0.750, 0.75, 0.125, 0.125),
        QRectF(0.875, 0.75, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.875, 0.125, 0.125),
        QRectF(0.125, 0.875, 0.125, 0.125),
        QRectF(0.25, 0.875, 0.125, 0.125),
        QRectF(0.375, 0.875, 0.125, 0.125),
        QRectF(0.5, 0.875, 0.125, 0.125),
        QRectF(0.625, 0.875, 0.125, 0.125),
        QRectF(0.750, 0.875, 0.125, 0.125),
        QRectF(0.875, 0.875, 0.125, 0.125)
      }}
    })));

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
    takeDamage->handlers.push_back(EventHandler{"entity_damaged",
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

    s = getValue(obj.dict, "st_patrolling_trigger", "");
    if (s != "") {
      agent->stPatrollingTrigger = Component::getIdFromString(s);
    }
    s = getValue(obj.dict, "st_chasing_trigger", "");
    if (s != "") {
      agent->stChasingTrigger = Component::getIdFromString(s);
    }
    agentSystem.addComponent(pComponent_t(agent));

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      parser::Object& child = **it;

      if (child.type == "patrol_path") {
        agent->patrolPath = child.path.points;
      }
    }

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
  static const set<string> types{"sprite", "bad_guy", "ammo"};
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
  else if (type == "ammo") {
    return constructAmmo(entityId, obj, region, parentTransform);
  }

  return false;
}
