#include "fragments/f_main/f_app_dialog/f_kernel/object_factory.hpp"
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


namespace millennium_bug {


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
  static const set<string> types{"computer_screen", "cell"};
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
    objects.insert(make_pair(entityId, parser::pObject_t(obj.clone())));
  }
  else {
    return m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform);
  }

  return true;
}

//===========================================
// ObjectFactory::constructObject
//===========================================
bool ObjectFactory::constructObject(const string& type, entityId_t entityId,
  parser::Object& obj, entityId_t region, const Matrix& parentTransform) {

  if (type == "computer_screen") {
    return constructComputerScreen(entityId, obj, region, parentTransform);
  }
  else if (type == "cell") {
    return constructCell(entityId, obj, region, parentTransform);
  }

  return false;
}


}
