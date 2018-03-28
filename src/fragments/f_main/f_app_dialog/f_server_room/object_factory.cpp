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

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("wall_decal", entityId, obj, parentId, parentTransform)) {
    AnimationSystem& animationSystem =
      m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);

    // Number of frames in sprite sheet
    const int W = 1;
    const int H = 3;

    CAnimation* anim = new CAnimation(entityId);

    vector<AnimationFrame> frames = constructFrames(W, H,
      { 0, 1, 2 });
    anim->animations.insert(std::make_pair("idle",
      Animation(m_timeService.frameRate, 1.0, frames)));

    animationSystem.addComponent(pComponent_t(anim));
    animationSystem.playAnimation(entityId, "idle", true);

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

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("wall_decal", entityId, obj, parentId, parentTransform)) {
    DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
    EventHandlerSystem& eventHandlerSystem =
      m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

    renderCalc();

    CDamage* damage = new CDamage(entityId, 1000, 1000);
    damageSystem.addComponent(pComponent_t(damage));

    CEventHandler* takeDamage = new CEventHandler(entityId);
    takeDamage->targetedEventHandlers.push_back(EventHandler{"entity_damaged",
      [=](const GameEvent& e) {

      const EEntityDamaged& event = dynamic_cast<const EEntityDamaged&>(e);
      DBG_PRINT("Calculator hit at " << event.point_rel << "\n");

      const Point& pt = event.point_rel;

      const CVRect& vRect = m_entityManager.getComponent<CVRect>(event.entityId,
        ComponentKind::C_SPATIAL);

      double W = m_wgtCalculator.width();
      double H = m_wgtCalculator.height();

      double x_norm = pt.x / vRect.size.x;
      double y_norm = pt.y / vRect.size.y;

      DBG_PRINT(x_norm << ", " << y_norm << "\n");

      double x = W - x_norm * W;
      double y = H - y_norm * H;

      DBG_PRINT(x << ", " << y << "\n");

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
    eventHandlerSystem.addComponent(pComponent_t(takeDamage));

    return true;
  }

  return false;
}

//===========================================
// ObjectFactory::ObjectFactory
//===========================================
ObjectFactory::ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager,
  AudioService& audioService, TimeService& timeService, CalculatorWidget& wgtCalculator)
  : m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_audioService(audioService),
    m_timeService(timeService),
    m_wgtCalculator(wgtCalculator) {}

//===========================================
// ObjectFactory::types
//===========================================
const set<string>& ObjectFactory::types() const {
  static const set<string> types{"big_screen", "calculator"};
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

  return false;
}


}
