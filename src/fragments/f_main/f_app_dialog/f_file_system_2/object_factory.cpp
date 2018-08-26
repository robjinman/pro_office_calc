#include "fragments/f_main/f_app_dialog/f_file_system_2/object_factory.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/c_elevator_behaviour.hpp"
#include "raycast/root_factory.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/time_service.hpp"
#include "raycast/audio_service.hpp"


using std::vector;
using std::set;
using std::string;


namespace t_minus_two_minutes {


//===========================================
// ObjectFactory::ObjectFactory
//===========================================
ObjectFactory::ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager,
  TimeService& timeService)
  : SystemAccessor(entityManager),
    m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_timeService(timeService) {}

//===========================================
// ObjectFactory::types
//===========================================
const set<string>& ObjectFactory::types() const {
  static const set<string> types{"covfefe", "cog", "smoke", "bridge_section"};
  return types;
}

//===========================================
// ObjectFactory::constructCovfefe
//===========================================
bool ObjectFactory::constructCovfefe(entityId_t entityId, parser::Object& obj, entityId_t region,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["texture"] = "covfefe";

  if (m_rootFactory.constructObject("collectable_item", entityId, obj, region, parentTransform)) {
    CFocus* focus = new CFocus(entityId);
    focus->captionText = "Press C to throw the covfefe";

    focusSys().addComponent(pComponent_t(focus));

    CEventHandler* events = nullptr;
    if (eventHandlerSys().hasComponent(entityId)) {
      events = &dynamic_cast<CEventHandler&>(eventHandlerSys().getComponent(entityId));
    }
    else {
      events = new CEventHandler(entityId);
      eventHandlerSys().addComponent(pComponent_t(events));
    }

    events->targetedEventHandlers.push_back(EventHandler{"item_collected",
      [this, entityId](const GameEvent&) {

      focusSys().showCaption(entityId);
    }});

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructBridgeSection
//===========================================
bool ObjectFactory::constructBridgeSection(entityId_t entityId, parser::Object& obj,
  entityId_t region, const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("elevator", entityId, obj, region, parentTransform)) {
    auto& behaviour = m_entityManager.getComponent<CElevatorBehaviour>(entityId,
      ComponentKind::C_BEHAVIOUR);

    CEventHandler* events = nullptr;
    if (eventHandlerSys().hasComponent(entityId)) {
      events = &dynamic_cast<CEventHandler&>(eventHandlerSys().getComponent(entityId));
    }
    else {
      events = new CEventHandler(entityId);
      eventHandlerSys().addComponent(pComponent_t(events));
    }

    events->broadcastedEventHandlers.push_back(EventHandler{"t_minus_two_minutes/machine_jammed",
      [&behaviour](const GameEvent&) {

      behaviour.move(0);
    }});

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructCog
//===========================================
bool ObjectFactory::constructCog(entityId_t entityId, parser::Object& obj, entityId_t region,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["texture"] = "cog";

  if (m_rootFactory.constructObject("wall_decal", entityId, obj, region, parentTransform)) {
    CAnimation* anim = new CAnimation(entityId);

    // Number of frames in sprite sheet
    const int W = 1;
    const int H = 8;

    vector<AnimationFrame> frames = constructFrames(W, H, { 0, 1, 2, 3, 4, 5, 6, 7 });
    anim->addAnimation(pAnimation_t(new Animation("idle", m_timeService.frameRate, 1.0, frames)));

    animationSys().addComponent(pComponent_t(anim));

    animationSys().playAnimation(entityId, "idle", true);

    CEventHandler* events = new CEventHandler(entityId);
    events->broadcastedEventHandlers.push_back(EventHandler{"t_minus_two_minutes/machine_jammed",
      [this, entityId](const GameEvent&) {

      animationSys().stopAnimation(entityId);
    }});
    events->targetedEventHandlers.push_back(EventHandler{"t_minus_two_minutes/covfefe_impact",
      [this](const GameEvent&) {

      m_entityManager.broadcastEvent(GameEvent{"t_minus_two_minutes/machine_jammed"});
    }});

    eventHandlerSys().addComponent(pComponent_t(events));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructSmoke
//===========================================
bool ObjectFactory::constructSmoke(entityId_t entityId, parser::Object& obj, entityId_t region,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  obj.dict["texture"] = "smoke";

  if (m_rootFactory.constructObject("sprite", entityId, obj, region, parentTransform)) {
    CAnimation* anim = new CAnimation(entityId);

    // Number of frames in sprite sheet
    const int W = 1;
    const int H = 3;

    vector<AnimationFrame> frames = constructFrames(W, H, { 0, 1, 2 });
    anim->addAnimation(pAnimation_t(new Animation("idle", m_timeService.frameRate, 1.0, frames)));

    animationSys().addComponent(pComponent_t(anim));

    animationSys().playAnimation(entityId, "idle", true);

    CEventHandler* events = new CEventHandler(entityId);
    events->broadcastedEventHandlers.push_back(EventHandler{"t_minus_two_minutes/machine_jammed",
      [this, entityId](const GameEvent&) {

      m_entityManager.deleteEntity(entityId);
    }});

    eventHandlerSys().addComponent(pComponent_t(events));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructObject
//===========================================
bool ObjectFactory::constructObject(const string& type, entityId_t entityId,
  parser::Object& obj, entityId_t region, const Matrix& parentTransform) {

  if (type == "covfefe") {
    return constructCovfefe(entityId, obj, region, parentTransform);
  }
  else if (type == "cog") {
    return constructCog(entityId, obj, region, parentTransform);
  }
  else if (type == "smoke") {
    return constructSmoke(entityId, obj, region, parentTransform);
  }
  else if (type == "bridge_section") {
    return constructBridgeSection(entityId, obj, region, parentTransform);
  }

  return false;
}


}
