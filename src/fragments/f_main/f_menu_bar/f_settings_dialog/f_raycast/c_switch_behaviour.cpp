#include <set>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/c_switch_behaviour.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "event.hpp"


using std::set;


//===========================================
// CSwitchBehaviour::CSwitchBehaviour
//===========================================
CSwitchBehaviour::CSwitchBehaviour(entityId_t entityId, EntityManager& entityManager,
  entityId_t target, bool toggleable, double toggleDelay)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_target(target),
    m_toggleable(toggleable),
    m_timer(toggleDelay) {

  CWallDecal* decal = getDecal();
  if (decal != nullptr) {
    decal->texRect = QRectF(0, 0, 0.5, 1);
  }
}

//===========================================
// CSwitchBehaviour::update
//===========================================
void CSwitchBehaviour::update() {}

//===========================================
// CSwitchBehaviour::handleEvent
//===========================================
void CSwitchBehaviour::handleEvent(const GameEvent& e_) {
  if (e_.name == "playerActivateEntity") {
    if (m_toggleable || m_state == ST_OFF) {
      if (m_timer.ready()) {
        GameEvent e("switchActivateEntity");
        m_entityManager.broadcastEvent(e, set<entityId_t>{m_target});

        CWallDecal* decal = getDecal();
        if (decal != nullptr) {
          decal->texRect = QRectF(0.5, 0, 0.5, 1);
        }

        switch (m_state) {
          case ST_OFF:
            m_state = ST_ON;
            break;
          case ST_ON:
            m_state = ST_OFF;
            break;
        }
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
