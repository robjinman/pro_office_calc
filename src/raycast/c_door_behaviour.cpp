#include "raycast/c_door_behaviour.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/audio_service.hpp"
#include "raycast/time_service.hpp"
#include "event.hpp"


using std::string;


//===========================================
// CDoorBehaviour::CDoorBehaviour
//===========================================
CDoorBehaviour::CDoorBehaviour(entityId_t entityId, EntityManager& entityManager,
  TimeService& timeService, AudioService& audioService)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_timeService(timeService),
    m_audioService(audioService),
    m_timer(5.0) {

  CZone& zone = entityManager.getComponent<CZone>(entityId, ComponentKind::C_SPATIAL);

  m_y0 = zone.floorHeight;
  m_y1 = zone.ceilingHeight;

  zone.ceilingHeight = zone.floorHeight + 0.1;
}

//===========================================
// CDoorBehaviour::playSound
//===========================================
void CDoorBehaviour::playSound() const {
  CZone& zone = m_entityManager.getComponent<CZone>(entityId(), ComponentKind::C_SPATIAL);
  const Point& pos = zone.edges.front()->lseg.A;

  m_audioService.playSoundAtPos("door", pos, true);
}

//===========================================
// CDoorBehaviour::stopSound
//===========================================
void CDoorBehaviour::stopSound() const {
  m_audioService.stopSound("door");
}

//===========================================
// CDoorBehaviour::update
//===========================================
void CDoorBehaviour::update() {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  Player& player = *spatialSystem.sg.player;
  CZone& zone = m_entityManager.getComponent<CZone>(entityId(), ComponentKind::C_SPATIAL);

  double dy = 60.0 / m_timeService.frameRate;

  switch (m_state) {
    case ST_CLOSED:
      return;
    case ST_OPEN:
      if (closeAutomatically && m_timer.ready()) {
        m_state = ST_CLOSING;
        playSound();
      }
      return;
    case ST_OPENING:
      zone.ceilingHeight += dy;

      if (zone.ceilingHeight + dy >= m_y1) {
        m_state = ST_OPEN;
        stopSound();
      }
      else {
        m_timer.reset();
      }

      break;
    case ST_CLOSING:
      zone.ceilingHeight -= dy;

      if (player.region() == entityId()) {
        if (zone.ceilingHeight - dy <= player.headHeight()) {
          m_state = ST_OPENING;
        }
      }
      else if (zone.ceilingHeight - dy <= m_y0) {
        m_state = ST_CLOSED;
        stopSound();
      }

      break;
  }
}

//===========================================
// CDoorBehaviour::handleBroadcastedEvent
//===========================================
void CDoorBehaviour::handleBroadcastedEvent(const GameEvent& e) {
  if (e.name == openOnEvent) {
    if (m_state != ST_OPEN) {
      m_state = ST_OPENING;
      playSound();
    }
  }
}

//===========================================
// CDoorBehaviour::handleTargetedEvent
//===========================================
void CDoorBehaviour::handleTargetedEvent(const GameEvent& e) {
  const std::set<std::string> EVENT_NAMES{
    "activate_entity",
    "player_activate_entity",
    "switch_activated"
  };

  if (!isPlayerActivated && e.name == "player_activate_entity") {
    auto& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);
    if (focusSystem.hasComponent(entityId())) {
      focusSystem.showCaption(entityId());
    }
    return;
  }

  if (EVENT_NAMES.count(e.name)) {
    playSound();

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
