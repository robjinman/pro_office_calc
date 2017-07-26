#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/c_door_behaviour.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_components.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_components.hpp"
#include "event.hpp"


//===========================================
// CDoorBehaviour::CDoorBehaviour
//===========================================
CDoorBehaviour::CDoorBehaviour(entityId_t entityId, CZone& zone, double frameRate)
  : CBehaviour(entityId),
    m_zone(zone),
    m_frameRate(frameRate) {

  m_y0 = zone.floorHeight;
  m_y1 = zone.ceilingHeight;

  zone.ceilingHeight = zone.floorHeight + 0.1;
}

//===========================================
// CDoorBehaviour::update
//===========================================
void CDoorBehaviour::update() {
  double dy = 60.0 / m_frameRate;

  switch (m_state) {
    case ST_CLOSED:
      return;
    case ST_OPEN:
      return;
    case ST_OPENING:
      m_zone.ceilingHeight += dy;

      if (m_zone.ceilingHeight + dy >= m_y1) {
        m_state = ST_OPEN;
      }
      break;
    case ST_CLOSING:
      m_zone.ceilingHeight -= dy;

      if (m_zone.ceilingHeight - dy <= m_y0) {
        m_state = ST_CLOSED;
      }
      break;
  }
}

//===========================================
// CDoorBehaviour::handleEvent
//===========================================
void CDoorBehaviour::handleEvent(const GameEvent& e) {
  if (e.name == "activateEntity") {
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
