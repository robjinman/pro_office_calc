#include "fragments/f_main/f_app_dialog/f_file_system/object_factory.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/root_factory.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/time_service.hpp"


using std::vector;
using std::set;
using std::string;


namespace going_in_circles {


//===========================================
// ObjectFactory::ObjectFactory
//===========================================
ObjectFactory::ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager,
  TimeService& timeService)
  : m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_timeService(timeService) {}

//===========================================
// ObjectFactory::types
//===========================================
const set<string>& ObjectFactory::types() const {
  static const set<string> types{"computer_screen", "jeff"};
  return types;
}

//===========================================
// ObjectFactory::constructComputerScreen
//===========================================
bool ObjectFactory::constructComputerScreen(entityId_t entityId, parser::Object& obj,
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
    RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
    FocusSystem& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);
    EventHandlerSystem& eventHandlerSystem =
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
    focus->hoverText = name.replace(0, 1, 1, asciiToUpper(name[0]));
    focusSystem.addComponent(pComponent_t(focus));

    CEventHandler* activateHandler = new CEventHandler(entityId);

    bool inCircles = false;
    activateHandler->targetedEventHandlers.push_back(
      EventHandler{"player_activate_entity", [=, &focusSystem](const GameEvent& e) mutable {
        if (inCircles) {
          focus->captionText = "\"Do you ever feel you're going in circles?\"";
        }
        else {
          focus->captionText = "\"People head North to die\"";
        }

        inCircles = !inCircles;
        focusSystem.showCaption(entityId);
      }});

    eventHandlerSystem.addComponent(pComponent_t(activateHandler));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructObject
//===========================================
bool ObjectFactory::constructObject(const string& type, entityId_t entityId,
  parser::Object& obj, entityId_t region, const Matrix& parentTransform) {

  if (type == "computer_screen") {
    return constructComputerScreen(entityId, obj, region, parentTransform);
  }
  else if (type == "jeff") {
    return constructJeff(entityId, obj, region, parentTransform);
  }

  return false;
}


}
