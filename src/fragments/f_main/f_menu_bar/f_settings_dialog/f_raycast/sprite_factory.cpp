#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/sprite_factory.hpp"
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
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/audio_service.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/time_service.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::string;
using std::set;


//===========================================
// SpriteFactory::constructAmmo
//===========================================
bool SpriteFactory::constructAmmo(entityId_t entityId, const parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  InventorySystem& inventorySystem =
    m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(parentId));

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  CVRect* vRect = new CVRect(entityId, zone.entityId(), Size(25, 15));
  Matrix m = transformFromTriangle(obj.path);
  vRect->setTransform(parentTransform * obj.transform * m);
  vRect->zone = &zone;

  spatialSystem.addComponent(pComponent_t(vRect));

  CSprite* sprite = new CSprite(entityId, zone.entityId(), "ammo");
  sprite->texViews = {
    QRectF(0, 0, 1, 1)
  };

  renderSystem.addComponent(pComponent_t(sprite));

  CCollectable* collectable = new CCollectable(entityId, "ammo", 5);
  inventorySystem.addComponent(pComponent_t(collectable));

  return true;
}

//===========================================
// SpriteFactory::constructBadGuy
//===========================================
bool SpriteFactory::constructBadGuy(entityId_t entityId, const parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  AnimationSystem& animationSystem =
    m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);
  DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
  BehaviourSystem& behaviourSystem =
    m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(parentId));

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  CVRect* vRect = new CVRect(entityId, zone.entityId(), Size(70, 70));
  Matrix m = transformFromTriangle(obj.path);
  vRect->setTransform(parentTransform * obj.transform * m);
  vRect->zone = &zone;

  spatialSystem.addComponent(pComponent_t(vRect));

  CSprite* sprite = new CSprite(entityId, zone.entityId(), "bad_guy");
  sprite->texViews = {
    QRectF(0, 0, 0.125, 0.125),
    QRectF(0.125, 0, 0.125, 0.125),
    QRectF(0.25, 0, 0.125, 0.125),
    QRectF(0.375, 0, 0.125, 0.125),
    QRectF(0.5, 0, 0.125, 0.125),
    QRectF(0.625, 0, 0.125, 0.125),
    QRectF(0.750, 0, 0.125, 0.125),
    QRectF(0.875, 0, 0.125, 0.125)
  };

  renderSystem.addComponent(pComponent_t(sprite));

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

  CDamage* damage = new CDamage(entityId, 2, 2);
  damageSystem.addComponent(pComponent_t(damage));

  CEventHandler* takeDamage = new CEventHandler(entityId);
  takeDamage->handlers.push_back(EventHandler{"entityDamaged",
    [=, &animationSystem](const GameEvent& e) {

    DBG_PRINT("Enemy health: " << damage->health << "\n");
    animationSystem.playAnimation(entityId, "hurt", false);

    if (damage->health == 0) {
      m_audioService.playSoundAtPos("monster_death", vRect->pos);
      m_entityManager.deleteEntity(entityId);
    }
    else {
      m_audioService.playSoundAtPos("monster_hurt", vRect->pos);
    }
  }});
  eventHandlerSystem.addComponent(pComponent_t(takeDamage));

  CEnemyBehaviour* behaviour = new CEnemyBehaviour(entityId, m_entityManager, m_audioService,
    m_timeService);

  string s = getValue(obj.dict, "st_patrolling_trigger", "");
  if (s != "") {
    behaviour->stPatrollingTrigger = Component::getIdFromString(s);
  }
  s = getValue(obj.dict, "st_chasing_trigger", "");
  if (s != "") {
    behaviour->stChasingTrigger = Component::getIdFromString(s);
  }
  behaviourSystem.addComponent(pComponent_t(behaviour));

  for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
    parser::Object& child = **it;
    string type = child.dict.at("type");

    if (type == "patrol_path") {
      behaviour->patrolPath = child.path.points;
    }
  }

  return true;
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
  static const set<string> types{"bad_guy", "ammo"};
  return types;
}

//===========================================
// SpriteFactory::constructObject
//===========================================
bool SpriteFactory::constructObject(const string& type, entityId_t entityId,
  const parser::Object& obj, entityId_t region, const Matrix& parentTransform) {

  if (type == "bad_guy") {
    return constructBadGuy(entityId, obj, region, parentTransform);
  }
  else if (type == "ammo") {
    return constructAmmo(entityId, obj, region, parentTransform);
  }

  return false;
}
