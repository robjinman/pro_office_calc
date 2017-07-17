#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/c_door_behaviour.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_objects.hpp"
#include "event.hpp"


//===========================================
// CDoorBehaviour::CDoorBehaviour
//===========================================
CDoorBehaviour::CDoorBehaviour(entityId_t entityId, Region& region)
  : CBehaviour(entityId),
    m_region(region) {

  m_y0 = region.floorHeight;
  m_y1 = region.ceilingHeight;

  region.ceilingHeight = region.floorHeight + 0.1;
}

//===========================================
// CDoorBehaviour::update
//===========================================
void CDoorBehaviour::update() {
  double dy = 1.0;

  switch (m_state) {
    case ST_CLOSED:
      return;
    case ST_OPEN:
      return;
    case ST_OPENING:
      m_region.ceilingHeight += dy;

      if (m_region.ceilingHeight + dy >= m_y1) {
        m_state = ST_OPEN;
      }
      break;
    case ST_CLOSING:
      m_region.ceilingHeight -= dy;

      if (m_region.ceilingHeight - dy <= m_y0) {
        m_state = ST_CLOSED;
      }
      break;
  }
}

//===========================================
// CDoorBehaviour::handleEvent
//===========================================
void CDoorBehaviour::handleEvent(const Event& e) {
  if (e.name == "raycast.playerActivate") {
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
