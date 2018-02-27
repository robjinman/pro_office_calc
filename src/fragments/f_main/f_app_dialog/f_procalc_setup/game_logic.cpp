#include <sstream>
#include <QDialog>
#include <QApplication>
#include "fragments/f_main/f_app_dialog/f_procalc_setup/game_logic.hpp"
#include "fragments/f_main/f_app_dialog/f_procalc_setup/events.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/c_switch_behaviour.hpp"
#include "raycast/c_elevator_behaviour.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "event_system.hpp"
#include "state_ids.hpp"
#include "request_state_change_event.hpp"
#include "utils.hpp"


using std::set;
using std::string;


namespace making_progress {


static const int MAX_KEY_PRESSES = 16;
static std::mt19937 randEngine(randomSeed());


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(QDialog& dialog, EventSystem& eventSystem, EntityManager& entityManager)
  : m_dialog(dialog),
    m_eventSystem(eventSystem),
    m_entityManager(entityManager),
    m_entityId(Component::getNextId()),
    m_raiseDialogEvent(static_cast<QEvent::Type>(QEvent::registerEventType())) {

  DBG_PRINT("GameLogic::GameLogic\n");

  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  pCEventHandler_t forwardEvent(new CEventHandler(m_entityId));

  forwardEvent->handlers.push_back(EventHandler{"entity_changed_zone",
    std::bind(&GameLogic::onEntityChangeZone, this, std::placeholders::_1)});

  eventHandlerSystem.addComponent(std::move(forwardEvent));

  m_eventSystem.listen("makingProgress/buttonPress", [this](const Event& event) {
    onButtonPress(event);
  }, m_eventIdx);
}

//===========================================
// GameLogic::setFeatures
//===========================================
void GameLogic::setFeatures(const set<buttonId_t>& features) {
  m_features = features;

  setElevatorSpeed();
  generateTargetNumber();
}

//===========================================
// GameLogic::generateTargetNumber
//===========================================
void GameLogic::generateTargetNumber() {
  std::uniform_int_distribution<int> randInt(100000, 999999);
  m_targetNumber = randInt(randEngine) / 100.0;

  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  Texture& tex = renderSystem.rg.textures.at("number");

  std::stringstream ss;
  ss << m_targetNumber;
  string strNumber = ss.str();

  QFont font;
  font.setPixelSize(14);

  QPainter painter(&tex.image);
  painter.setFont(font);
  painter.setPen(QColor(0, 255, 0));

  tex.image.fill(QColor(20, 20, 50));
  painter.drawText(8, 16, strNumber.c_str());

  Texture& remainingTex = renderSystem.rg.textures.at("keypresses_remaining");
  remainingTex.image.fill(QColor(20, 20, 50));
}

//===========================================
// GameLogic::onButtonPress
//===========================================
void GameLogic::onButtonPress(const Event& event) {
  if (m_success) {
    return;
  }

  const ButtonPressEvent& e = dynamic_cast<const ButtonPressEvent&>(event);

  double value = 0;
  std::stringstream ss(e.calcDisplay);

  if (ss >> value) {
    if (fabs(value - m_targetNumber) < 0.001) {
      entityId_t doorId = Component::getIdFromString("exit_door");
      EActivateEntity e;

      m_success = true;
      m_entityManager.fireEvent(e, {doorId});
      return;
    }
  }

  ++m_numKeysPressed;

  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  Texture& tex = renderSystem.rg.textures.at("keypresses_remaining");

  tex.image.fill(QColor(20, 20, 50));

  if (m_numKeysPressed > MAX_KEY_PRESSES) {
    generateTargetNumber();
    m_numKeysPressed = 0;
  }
  else {
    int w = tex.image.width();
    int h = tex.image.height();
    double delta = static_cast<double>(w) / (MAX_KEY_PRESSES + 1);

    QPainter painter(&tex.image);
    painter.setBrush(QColor(240, 10, 10));
    painter.drawRect(0, 0, delta * m_numKeysPressed, h);
  }
}

//===========================================
// GameLogic::setElevatorSpeed
//===========================================
void GameLogic::setElevatorSpeed() {
  entityId_t elevatorId = Component::getIdFromString("progress_lift");

  CElevatorBehaviour& elevator =
    m_entityManager.getComponent<CElevatorBehaviour>(elevatorId, ComponentKind::C_BEHAVIOUR);

  const double MIN_SPEED = 2;
  const double MAX_SPEED = 40;
  const double MAX_FEATURES = 17;
  double delta = (MAX_SPEED - MIN_SPEED) / MAX_FEATURES;

  elevator.setSpeed(MAX_SPEED - delta * m_features.size());
}

//===========================================
// GameLogic::customEvent
//===========================================
void GameLogic::customEvent(QEvent* event) {
  if (event->type() == m_raiseDialogEvent) {
    m_dialog.activateWindow();
  }
}

//===========================================
// GameLogic::onEntityChangeZone
//===========================================
void GameLogic::onEntityChangeZone(const GameEvent& event) {
  const EChangedZone& e = dynamic_cast<const EChangedZone&>(event);

  entityId_t player =
    m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL).sg.player->body;

  if (e.entityId != player) {
    return;
  }

  if (e.newZone == Component::getIdFromString("puzzle_room_entrance")) {
    m_eventSystem.fire(pEvent_t(new SetupCompleteEvent(m_features)));

    QApplication::postEvent(this, new QEvent(m_raiseDialogEvent));
  }
  else if (e.newZone == Component::getIdFromString("level_exit")) {
    m_eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_YOUVE_GOT_MAIL)));
  }
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {
  m_eventSystem.forget(m_eventIdx);
  m_entityManager.deleteEntity(m_entityId);
}


}
