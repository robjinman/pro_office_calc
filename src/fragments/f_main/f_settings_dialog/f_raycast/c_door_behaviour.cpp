#include "fragments/f_main/f_settings_dialog/f_raycast/c_door_behaviour.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/render_components.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "event.hpp"


//===========================================
// CDoorBehaviour::CDoorBehaviour
//===========================================
CDoorBehaviour::CDoorBehaviour(entityId_t entityId, EntityManager& entityManager, double frameRate)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_frameRate(frameRate),
    m_timer(5.0) {

  CZone& zone = entityManager.getComponent<CZone>(entityId, ComponentKind::C_SPATIAL);

  m_y0 = zone.floorHeight;
  m_y1 = zone.ceilingHeight;

  zone.ceilingHeight = zone.floorHeight + 0.1;
}

//===========================================
// CDoorBehaviour::update
//===========================================
void CDoorBehaviour::update() {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  Player& player = *spatialSystem.sg.player;
  CZone& zone = m_entityManager.getComponent<CZone>(entityId(), ComponentKind::C_SPATIAL);

  double dy = 60.0 / m_frameRate;

  switch (m_state) {
    case ST_CLOSED:
      return;
    case ST_OPEN:
      if (m_timer.ready()) {
        m_state = ST_CLOSING;
      }
      return;
    case ST_OPENING:
      zone.ceilingHeight += dy;

      if (zone.ceilingHeight + dy >= m_y1) {
        m_state = ST_OPEN;
      }
      else {
        m_timer.reset();
      }

      break;
    case ST_CLOSING:
      zone.ceilingHeight -= dy;

      if (player.currentRegion == entityId()) {
        if (zone.ceilingHeight - dy <= player.headHeight()) {
          m_state = ST_OPENING;
        }
      }
      else if (zone.ceilingHeight - dy <= m_y0) {
        m_state = ST_CLOSED;
      }

      break;
  }
}

//===========================================
// CDoorBehaviour::handleEvent
//===========================================
void CDoorBehaviour::handleEvent(const GameEvent& e) {
  if (e.name == "playerActivateEntity" || e.name == "switchActivateEntity") {
    switch (m_state) {
      case ST_CLOSED:
        m_state = ST_OPENING;
        break;
      case ST_OPEN:
        m_state = ST_CLOSING;
        break;
      case ST_OPENING:
        m_state = ST_CLOSING;
        break;
      case ST_CLOSING:
        m_state = ST_OPENING;
        break;
    }
  }
}
