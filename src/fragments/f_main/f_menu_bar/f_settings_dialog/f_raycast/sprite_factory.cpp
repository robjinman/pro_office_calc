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
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/audio_manager.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::string;


//===========================================
// constructAmmo
//===========================================
static void constructAmmo(EntityManager& em, const parser::Object& obj, CZone& zone,
  CRegion& region, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = em.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = em.system<RenderSystem>(ComponentKind::C_RENDER);
  InventorySystem& inventorySystem = em.system<InventorySystem>(ComponentKind::C_INVENTORY);

  entityId_t id = Component::getNextId();

  CVRect* vRect = new CVRect(id, zone.entityId(), Size(25, 15));
  Matrix m = transformFromTriangle(obj.path);
  vRect->setTransform(parentTransform * obj.transform * m);
  vRect->zone = &zone;

  spatialSystem.addComponent(pComponent_t(vRect));

  CSprite* sprite = new CSprite(id, zone.entityId(), "ammo");
  sprite->texViews = {
    QRectF(0, 0, 1, 1)
  };

  renderSystem.addComponent(pComponent_t(sprite));

  CCollectable* collectable = new CCollectable(id, "ammo", 5);
  inventorySystem.addComponent(pComponent_t(collectable));
}

//===========================================
// constructBadGuy
//===========================================
static void constructBadGuy(EntityManager& em, AudioManager& audioManager,
  const parser::Object& obj, double frameRate, CZone& zone, CRegion& region,
  const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = em.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = em.system<RenderSystem>(ComponentKind::C_RENDER);
  AnimationSystem& animationSystem = em.system<AnimationSystem>(ComponentKind::C_ANIMATION);
  DamageSystem& damageSystem = em.system<DamageSystem>(ComponentKind::C_DAMAGE);
  EventHandlerSystem& eventHandlerSystem =
    em.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
  BehaviourSystem& behaviourSystem = em.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

  entityId_t id = Component::getNextId();

  CVRect* vRect = new CVRect(id, zone.entityId(), Size(70, 70));
  Matrix m = transformFromTriangle(obj.path);
  vRect->setTransform(parentTransform * obj.transform * m);
  vRect->zone = &zone;

  spatialSystem.addComponent(pComponent_t(vRect));

  CSprite* sprite = new CSprite(id, zone.entityId(), "bad_guy");
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

  CAnimation* anim = new CAnimation(id);
  anim->animations.insert(std::make_pair("idle", Animation(frameRate, 1.0, {
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
  animationSystem.playAnimation(id, "idle", true);

  CDamage* damage = new CDamage(id, 5, 5);
  damageSystem.addComponent(pComponent_t(damage));

  CEventHandler* takeDamage = new CEventHandler(id);
  takeDamage->handlers.push_back(EventHandler{"entityDamaged",
    [=, &em, &audioManager, &animationSystem](const GameEvent& e) {

    DBG_PRINT("Enemy health: " << damage->health << "\n");
    animationSystem.playAnimation(id, "hurt", false);

    if (damage->health == 0) {
      audioManager.playSoundAtPos("monster_death", vRect->pos);
      em.deleteEntity(id);
    }
    else {
      audioManager.playSoundAtPos("monster_hurt", vRect->pos);
    }
  }});
  eventHandlerSystem.addComponent(pComponent_t(takeDamage));

  CEnemyBehaviour* behaviour = new CEnemyBehaviour(id, em, frameRate);
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
}

//===========================================
// constructSprite
//===========================================
void constructSprite(EntityManager& em, AudioManager& audioManager, const parser::Object& obj,
  double frameRate, CZone& zone, CRegion& region, const Matrix& parentTransform) {

  DBG_PRINT("Constructing Sprite\n");

  if (getValue(obj.dict, "subtype") == "bad_guy") {
    constructBadGuy(em, audioManager, obj, frameRate, zone, region, parentTransform);
  }
  else if (getValue(obj.dict, "subtype") == "ammo") {
    constructAmmo(em, obj, zone, region, parentTransform);
  }
  else {
    EXCEPTION("Error constructing sprite of unknown type");
  }
}
