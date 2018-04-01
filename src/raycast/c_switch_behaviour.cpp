#include <set>
#include "raycast/c_switch_behaviour.hpp"
#include "raycast/render_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/time_service.hpp"
#include "event.hpp"


using std::string;
using std::set;
using std::map;


//===========================================
// CSwitchBehaviour::CSwitchBehaviour
//===========================================
CSwitchBehaviour::CSwitchBehaviour(entityId_t entityId, EntityManager& entityManager,
  TimeService& timeService, entityId_t target, const string& message, SwitchState initialState,
  bool toggleable, double toggleDelay)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_timeService(timeService),
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
// CSwitchBehaviour::handleTargetedEvent
//===========================================
void CSwitchBehaviour::handleTargetedEvent(const GameEvent& e_) {
  if (e_.name == "player_activate_entity") {
    const EPlayerActivateEntity& e = dynamic_cast<const EPlayerActivateEntity&>(e_);

    if (m_toggleable || m_state == SwitchState::OFF) {
      if (m_timer.ready()) {
        const auto& inventorySystem = m_entityManager
          .system<InventorySystem>(ComponentKind::C_INVENTORY);

        if (requiredItemType != "") {
          const map<string, entityId_t>& items =
            inventorySystem.getBucketItems(e.player.body, requiredItemType);

          if (!contains(items, requiredItemName) && caption != "") {
            showCaption();
            return;
          }
        }

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
        m_entityManager.fireEvent(eActivate, {m_target});
        m_entityManager.broadcastEvent(eActivate);
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

//===========================================
// CSwitchBehaviour::deleteCaption
//===========================================
void CSwitchBehaviour::deleteCaption() {
  m_timeService.cancelTimeout(m_captionTimeoutId);
  m_entityManager.deleteEntity(m_captionBgId);
  m_entityManager.deleteEntity(m_captionTextId);

  m_captionBgId = -1;
  m_captionTextId = -1;
  m_captionTimeoutId = -1;
}

//===========================================
// CSwitchBehaviour::showCaption
//===========================================
void CSwitchBehaviour::showCaption() {
  deleteCaption();

  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  const Size& vp = renderSystem.rg.viewport;

  double margin = 0.2;
  double chH = 0.8;
  double chW = 0.4;
  Size sz(2.0 * margin + chW * caption.length(), 2.0 * margin + chH);
  Point bgPos(0.5 * (vp.x - sz.x), 0.75 * (vp.y - sz.y));
  Point textPos = bgPos + Vec2f(margin, margin);

  QColor bgColour(0, 0, 0, 100);
  QColor textColour(200, 200, 0);

  m_captionBgId = Component::getNextId();
  m_captionTextId = Component::getNextId();

  CColourOverlay* bgOverlay = new CColourOverlay(m_captionBgId, bgColour, bgPos, sz, 8);
  CTextOverlay* textOverlay = new CTextOverlay(m_captionTextId, caption, textPos, chH, textColour,
    9);

  renderSystem.addComponent(pComponent_t(bgOverlay));
  renderSystem.addComponent(pComponent_t(textOverlay));

  m_captionTimeoutId = m_timeService.onTimeout([this]() {
    deleteCaption();
  }, 3.0);
}
