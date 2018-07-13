#include "fragments/f_main/f_app_dialog/f_file_system_2/object_factory.hpp"
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


namespace t_minus_two_minutes {


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
  static const set<string> types{"covfefe"};
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
    auto& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);
    auto& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

    CFocus* focus = new CFocus(entityId);
    focus->captionText = "Press C to throw the covfefe";

    focusSystem.addComponent(pComponent_t(focus));

    CEventHandler* events = nullptr;
    if (eventHandlerSystem.hasComponent(entityId)) {
      events = &dynamic_cast<CEventHandler&>(eventHandlerSystem.getComponent(entityId));
    }
    else {
      events = new CEventHandler(entityId);
      eventHandlerSystem.addComponent(pComponent_t(events));
    }

    events->targetedEventHandlers.push_back(EventHandler{"item_collected",
      [entityId, &focusSystem](const GameEvent&) {

      focusSystem.showCaption(entityId);
    }});

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

  return false;
}


}
