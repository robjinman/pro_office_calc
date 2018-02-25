#include <set>
#include "raycast/c_switch_behaviour.hpp"
#include "raycast/render_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "event.hpp"


using std::string;
using std::set;


//===========================================
// CSwitchBehaviour::CSwitchBehaviour
//===========================================
CSwitchBehaviour::CSwitchBehaviour(entityId_t entityId, EntityManager& entityManager,
  entityId_t target, const string& message, SwitchState initialState, bool toggleable,
  double toggleDelay)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_target(target),
    m_message(message),
    m_state(initialState),
    m_toggleable(toggleable),
    m_timer(toggleDelay) {

  setDecal();
}

//===========================================
// CSwitchBehaviour::update
//===========================================
void CSwitchBehaviour::update() {}

//===========================================
// CSwitchBehaviour::setDecal
//===========================================
void CSwitchBehaviour::setDecal() {
  QRectF texRect;

  switch (m_state) {
    case SwitchState::OFF:
      texRect = QRectF(0, 0, 0.5, 1);
      break;
    case SwitchState::ON:
      texRect = QRectF(0.5, 0, 0.5, 1);
      break;
  }

  CWallDecal* decal = getDecal();
  if (decal != nullptr) {
    decal->texRect = texRect;
  }
}

//===========================================
// CSwitchBehaviour::handleEvent
//===========================================
void CSwitchBehaviour::handleEvent(const GameEvent& e) {
  if (e.name == "player_activate_entity") {
    if (m_toggleable || m_state == SwitchState::OFF) {
      if (m_timer.ready()) {
        switch (m_state) {
          case SwitchState::OFF:
            m_state = SwitchState::ON;
            break;
          case SwitchState::ON:
            m_state = SwitchState::OFF;
            break;
        }

        setDecal();

        ESwitchActivate eActivate(entityId(), m_state, m_message);
        m_entityManager.broadcastEvent(eActivate);

        ESwitchActivateEntity eActivateEntity(entityId(), m_state, m_message);
        m_entityManager.fireEvent(eActivateEntity, set<entityId_t>{m_target});
      }
    }
  }
}

//===========================================
// CSwitchBehaviour::getDecal
//===========================================
CWallDecal* CSwitchBehaviour::getDecal() const {
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  if (renderSystem.hasComponent(entityId())) {
    return dynamic_cast<CWallDecal*>(&renderSystem.getComponent(entityId()));
  }

  return nullptr;
}
