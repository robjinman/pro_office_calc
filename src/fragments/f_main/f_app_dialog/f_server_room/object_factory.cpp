#include <cassert>
#include <QMouseEvent>
#include <QApplication>
#include "fragments/f_main/f_app_dialog/f_server_room/object_factory.hpp"
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
#include "calculator_widget.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::vector;
using std::string;
using std::set;


namespace youve_got_mail {


//===========================================
// ObjectFactory::constructBigScreen
//===========================================
bool ObjectFactory::constructBigScreen(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  assert(entityId == -1);
  entityId = Component::getIdFromString("big_screen");
  entityId_t calculatorId = Component::getIdFromString("calculator");

  if (m_rootFactory.constructObject("wall_decal", entityId, obj, parentId, parentTransform)) {
    AnimationSystem& animationSystem =
      m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);
    EventHandlerSystem& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

    // Number of frames in sprite sheet
    const int W = 1;
    const int H = 13;

    CAnimation* anim = new CAnimation(entityId);

    vector<AnimationFrame> frames = constructFrames(W, H,
      { 0, 1, 2, 1 });
    anim->addAnimation(pAnimation_t(new Animation("idle", m_timeService.frameRate, 2.0, frames)));

    frames = constructFrames(W, H,
      { 3, 4, 5, 4 });
    anim->addAnimation(pAnimation_t(new Animation("panic", m_timeService.frameRate, 2.0, frames)));

    frames = constructFrames(W, H,
      { 6, 7, 8, 9, 10, 11, 12 });
    anim->addAnimation(pAnimation_t(new Animation("escape", m_timeService.frameRate, 3.5, frames)));

    animationSystem.addComponent(pComponent_t(anim));
    animationSystem.playAnimation(entityId, "idle", true);

    CEventHandler* handlers = new CEventHandler(entityId);
    handlers->targetedEventHandlers.push_back(EventHandler{"animation_finished",
      [=, &animationSystem](const GameEvent& e) {

      const EAnimationFinished& event = dynamic_cast<const EAnimationFinished&>(e);

      if (event.animName == "panic") {
        m_entityManager.deleteEntity(calculatorId);
        animationSystem.playAnimation(entityId, "escape", false);
      }
      else if (event.animName == "escape") {
        m_timeService.onTimeout([=]() {
          m_entityManager.broadcastEvent(GameEvent("enter_larry"));
        }, 2.0);
      }
    }});
    handlers->broadcastedEventHandlers.push_back(EventHandler{"div_by_zero",
      [=, &animationSystem](const GameEvent&) {

      animationSystem.stopAnimation(entityId);
      animationSystem.playAnimation(entityId, "panic", false);
    }});
    eventHandlerSystem.addComponent(pComponent_t(handlers));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::constructServerRack
//===========================================
bool ObjectFactory::constructServerRack(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform)) {
    EventHandlerSystem& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
    DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
    AnimationSystem& animationSystem =
      m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);
    RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

    // Number of frames in sprite sheet
    const int W = 1;
    const int H = 8;

    vector<AnimationFrame> damageFrames = constructFrames(W, H, { 1, 0 });
    CAnimation* anim = new CAnimation(entityId);
    anim->addAnimation(pAnimation_t(new Animation("damage", m_timeService.frameRate, 0.3,
      damageFrames)));

    vector<AnimationFrame> explodeFrames = constructFrames(W, H, { 2, 3, 4, 5, 6, 7 });
    anim->addAnimation(pAnimation_t(new Animation("explode", m_timeService.frameRate, 0.6,
      explodeFrames)));

    animationSystem.addComponent(pComponent_t(anim));

    auto& children = renderSystem.children(entityId);
    for (entityId_t childId : children) {
      CAnimation* anim = new CAnimation(childId);
      anim->addAnimation(pAnimation_t(new Animation("damage", m_timeService.frameRate, 0.3,
        damageFrames)));
      anim->addAnimation(pAnimation_t(new Animation("explode", m_timeService.frameRate, 0.6,
        explodeFrames)));
      animationSystem.addComponent(pComponent_t(anim));
    }

    // Play once just to setup the initial texRect
    animationSystem.playAnimation(entityId, "damage", false);

    CDamage* damage = new CDamage(entityId, 3, 3);
    damageSystem.addComponent(pComponent_t(damage));

    CEventHandler* handlers = new CEventHandler(entityId);
    handlers->targetedEventHandlers.push_back(EventHandler{"entity_damaged",
      [=, &animationSystem](const GameEvent& e) {

      animationSystem.playAnimation(entityId, "damage", false);
    }});
    handlers->targetedEventHandlers.push_back(EventHandler{"entity_destroyed",
      [=, &animationSystem](const GameEvent& e) {

      animationSystem.playAnimation(entityId, "explode", false);
      m_audioService.playSound("electricity");
      m_entityManager.broadcastEvent(GameEvent("server_destroyed"));
    }});
    handlers->targetedEventHandlers.push_back(EventHandler{"animation_finished",
      [this](const GameEvent& e_) {

      auto& e = dynamic_cast<const EAnimationFinished&>(e_);
      if (e.animName == "explode") {
        m_audioService.stopSound("electricity");
      }
    }});
    eventHandlerSystem.addComponent(pComponent_t(handlers));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::renderCalc
//===========================================
void ObjectFactory::renderCalc() const {
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  int W = m_wgtCalculator.width();
  int H = m_wgtCalculator.height();

  renderSystem.rg.textures["calculator"] = Texture{QImage(W, H, QImage::Format_ARGB32),
    Size(0, 0)};

  QImage& calcImg = renderSystem.rg.textures["calculator"].image;
  calcImg.fill(QColor(255, 0, 0));

  QImage buf(W, H, QImage::Format_ARGB32);
  m_wgtCalculator.render(&buf);

  QPainter painter;
  painter.begin(&calcImg);

  QTransform t = painter.transform();
  t.scale(-1, 1);
  painter.setTransform(t);

  painter.drawImage(QPoint(-W, 0), buf);

  painter.end();
}

//===========================================
// ObjectFactory::constructCalculator
//===========================================
bool ObjectFactory::constructCalculator(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  assert(entityId == -1);
  entityId = Component::getIdFromString("calculator");

  if (m_rootFactory.constructObject("wall_decal", entityId, obj, parentId, parentTransform)) {
    DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
    EventHandlerSystem& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

    renderCalc();

    CDamage* damage = new CDamage(entityId, 1000, 1000);
    damageSystem.addComponent(pComponent_t(damage));

    CEventHandler* handlers = new CEventHandler(entityId);
    handlers->targetedEventHandlers.push_back(EventHandler{"entity_damaged",
      [=](const GameEvent& e) {

      const EEntityDamaged& event = dynamic_cast<const EEntityDamaged&>(e);
      const Point& pt = event.point_rel;

      const CVRect& vRect = m_entityManager.getComponent<CVRect>(event.entityId,
        ComponentKind::C_SPATIAL);

      double W = m_wgtCalculator.width();
      double H = m_wgtCalculator.height();

      double x_norm = pt.x / vRect.size.x;
      double y_norm = pt.y / vRect.size.y;

      double x = W - x_norm * W;
      double y = H - y_norm * H;

      QWidget* child = m_wgtCalculator.childAt(x, y);
      if (child != nullptr) {
        QMouseEvent mousePressEvent(QEvent::MouseButtonPress, QPointF(0, 0), Qt::LeftButton,
          Qt::LeftButton, Qt::NoModifier);

        QMouseEvent mouseReleaseEvent(QEvent::MouseButtonRelease, QPointF(0, 0), Qt::LeftButton,
          Qt::NoButton, Qt::NoModifier);

        QApplication::sendEvent(child, &mousePressEvent);
        QApplication::sendEvent(child, &mouseReleaseEvent);
      }

      renderCalc();

      if (m_wgtCalculator.wgtDigitDisplay->text() == "inf") {
        m_entityManager.broadcastEvent(GameEvent("div_by_zero"));
      }
    }});
    eventHandlerSystem.addComponent(pComponent_t(handlers));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::ObjectFactory
//===========================================
ObjectFactory::ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager,
  TimeService& timeService, AudioService& audioService, CalculatorWidget& wgtCalculator)
  : m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_timeService(timeService),
    m_audioService(audioService),
    m_wgtCalculator(wgtCalculator) {}

//===========================================
// ObjectFactory::types
//===========================================
const set<string>& ObjectFactory::types() const {
  static const set<string> types{"big_screen", "calculator", "server_rack"};
  return types;
}

//===========================================
// ObjectFactory::constructObject
//===========================================
bool ObjectFactory::constructObject(const string& type, entityId_t entityId,
  parser::Object& obj, entityId_t region, const Matrix& parentTransform) {

  if (type == "big_screen") {
    return constructBigScreen(entityId, obj, region, parentTransform);
  }
  else if (type == "calculator") {
    return constructCalculator(entityId, obj, region, parentTransform);
  }
  else if (type == "server_rack") {
    return constructServerRack(entityId, obj, region, parentTransform);
  }

  return false;
}


}
