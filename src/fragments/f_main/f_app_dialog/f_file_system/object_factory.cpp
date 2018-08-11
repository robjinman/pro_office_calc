#include "fragments/f_main/f_app_dialog/f_file_system/object_factory.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/root_factory.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/time_service.hpp"
#include "raycast/audio_service.hpp"


using std::vector;
using std::set;
using std::string;


namespace going_in_circles {


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
  static const set<string> types{"jeff", "donald"};
  return types;
}

//===========================================
// ObjectFactory::constructJeff
//===========================================
bool ObjectFactory::constructJeff(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  string name = obj.dict["name"] = "jeff";

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["texture"] = "jeff";

  if (m_rootFactory.constructObject("sprite", entityId, obj, parentId, parentTransform)) {
    auto& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
    auto& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);
    auto& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
    auto& animationSystem = m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);
    auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
    auto& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);

    CSprite& sprite = dynamic_cast<CSprite&>(renderSystem.getComponent(entityId));

    double H = 2.0;
    double W = 8.0;
    double dW = 1.0 / W;

    vector<QRectF> texViews = {
      QRectF(dW * 0.0, 0, dW, 1.0 / H),
      QRectF(dW * 1.0, 0, dW, 1.0 / H),
      QRectF(dW * 2.0, 0, dW, 1.0 / H),
      QRectF(dW * 3.0, 0, dW, 1.0 / H),
      QRectF(dW * 4.0, 0, dW, 1.0 / H),
      QRectF(dW * 5.0, 0, dW, 1.0 / H),
      QRectF(dW * 6.0, 0, dW, 1.0 / H),
      QRectF(dW * 7.0, 0, dW, 1.0 / H)
    };
    sprite.texViews = texViews;

    CDamage* damage = new CDamage(entityId, 2, 2);
    damageSystem.addComponent(pComponent_t(damage));

    CFocus* focus = new CFocus(entityId);
    focus->hoverText = name.replace(0, 1, 1, asciiToUpper(name[0]));
    focusSystem.addComponent(pComponent_t(focus));

    CAnimation* anim = new CAnimation(entityId);

    vector<AnimationFrame> frames{{
      AnimationFrame{{QRectF{0.0 / W, 1.0 / H, 1.0 / W, 1.0 / H}}},
      AnimationFrame{{QRectF{1.0 / W, 1.0 / H, 1.0 / W, 1.0 / H}}},
      AnimationFrame{{QRectF{2.0 / W, 1.0 / H, 1.0 / W, 1.0 / H}}},
      AnimationFrame{{QRectF{3.0 / W, 1.0 / H, 1.0 / W, 1.0 / H}}},
    }};
    anim->addAnimation(pAnimation_t(new Animation("death", m_timeService.frameRate, 0.5, frames)));

    animationSystem.addComponent(pComponent_t(anim));

    CEventHandler* events = new CEventHandler(entityId);

    bool inCircles = false;
    events->targetedEventHandlers.push_back(EventHandler{"player_activate_entity",
      [=, &focusSystem, &damageSystem](const GameEvent&) mutable {

      if (damageSystem.getHealth(entityId) != 0) {
        if (inCircles) {
          focus->captionText = "\"Do you ever feel you're going in circles?\"";
        }
        else {
          focus->captionText = "\"People head North to die\"";
        }

        inCircles = !inCircles;
      }
      else {
        focus->captionText = "\"The snacks here are pretty good\"";
      }

      focusSystem.showCaption(entityId);
    }});
    events->targetedEventHandlers.push_back(EventHandler{"entity_damaged",
      [=, &animationSystem, &spatialSystem](const GameEvent&) {

      CVRect& body = dynamic_cast<CVRect&>(spatialSystem.getComponent(entityId));

      DBG_PRINT("Jeff health: " << damage->health << "\n");
      //animationSystem.playAnimation(entityId, "hurt", false);

      if (damage->health == 0) {
        m_audioService.playSoundAtPos("civilian_death", body.pos);
        animationSystem.playAnimation(entityId, "death", false);
      }
      else {
        m_audioService.playSoundAtPos("civilian_hurt", body.pos);
      }
    }});
    events->targetedEventHandlers.push_back(EventHandler{"animation_finished",
      [=, &spatialSystem, &sprite](const GameEvent& e_) {

      auto& e = dynamic_cast<const EAnimationFinished&>(e_);
      if (e.animName == "death") {
        entityId_t spawnPointId = Component::getIdFromString("jeff_spawn_point");
        CVRect& spawnPoint = dynamic_cast<CVRect&>(spatialSystem.getComponent(spawnPointId));

        spatialSystem.relocateEntity(entityId, *spawnPoint.zone, spawnPoint.pos);

        sprite.texViews = texViews;
      }
    }});

    eventHandlerSystem.addComponent(pComponent_t(events));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructDonald
//===========================================
bool ObjectFactory::constructDonald(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  string name = obj.dict["name"] = "donald";

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["texture"] = "donald";

  if (m_rootFactory.constructObject("sprite", entityId, obj, parentId, parentTransform)) {
    auto& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
    auto& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);
    auto& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

    CSprite& sprite = dynamic_cast<CSprite&>(renderSystem.getComponent(entityId));

    double W = 8.0;
    double dW = 1.0 / W;

    sprite.texViews = {
      QRectF(dW * 0.0, 0, dW, 1),
      QRectF(dW * 1.0, 0, dW, 1),
      QRectF(dW * 2.0, 0, dW, 1),
      QRectF(dW * 3.0, 0, dW, 1),
      QRectF(dW * 4.0, 0, dW, 1),
      QRectF(dW * 5.0, 0, dW, 1),
      QRectF(dW * 6.0, 0, dW, 1),
      QRectF(dW * 7.0, 0, dW, 1)
    };

    CFocus* focus = new CFocus(entityId);
    focus->hoverText = "Donald";
    focus->captionText = "\"Have you seen my product? It's a great product\"";
    focusSystem.addComponent(pComponent_t(focus));

    CEventHandler* events = new CEventHandler(entityId);

    events->targetedEventHandlers.push_back(EventHandler{"player_activate_entity",
      [=, &focusSystem](const GameEvent&) {

      focusSystem.showCaption(entityId);
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

  if (type == "jeff") {
    return constructJeff(entityId, obj, region, parentTransform);
  }
  else if (type == "donald") {
    return constructDonald(entityId, obj, region, parentTransform);
  }

  return false;
}


}
