#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/c_enemy_behaviour.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/damage_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_components.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/audio_manager.hpp"
#include "event.hpp"


using std::vector;
using std::list;


//===========================================
// indexOfClosestPoint
//===========================================
static int indexOfClosestPoint(const Point& point, const vector<Point>& points) {
  int idx = 0;
  double closest = 1000000;

  for (unsigned int i = 0; i < points.size(); ++i) {
    double d = distance(point, points[i]);
    if (d < closest) {
      closest = d;
      idx = i;
    }
  }

  return idx;
}

//===========================================
// hasLineOfSight
//===========================================
static bool hasLineOfSight(SpatialSystem& spatialSystem, const CVRect& body, const Player& player) {
  const Point& target_wld = player.body.pos;

  double targetHeight = player.feetHeight() + 0.5 * player.getTallness();
  Vec2f v = target_wld - body.pos;
  double hAngle = atan2(v.y, v.x);

  Matrix t(hAngle, body.pos);
  Matrix m = t.inverse();

  Point target_rel = m * target_wld;
  Vec2f ray = normalise(target_rel);

  double height = body.zone->floorHeight + body.size.y * 0.5;
  double vAngle = atan2(targetHeight - height, length(target_rel));

  list<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(*body.zone, ray * 0.00001,
    height, ray, vAngle, m);

  if (intersections.size() > 0) {
    entityId_t id = intersections.front()->entityId;
    if (id == player.body.entityId()) {
      return true;
    }
  }

  return false;
}

//===========================================
// CEnemyBehaviour::CEnemyBehaviour
//===========================================
CEnemyBehaviour::CEnemyBehaviour(entityId_t entityId, EntityManager& entityManager,
  AudioManager& audioManager, double frameRate)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_audioManager(audioManager),
    m_frameRate(frameRate) {

  m_gunfireTiming.reset(new TRandomIntervals(400, 4000));
}

//===========================================
// CEnemyBehaviour::doPatrollingBehaviour
//===========================================
void CEnemyBehaviour::doPatrollingBehaviour(SpatialSystem& spatialSystem, CVRect& body) {
  if (m_waypointIdx == -1) {
    m_waypointIdx = indexOfClosestPoint(body.pos, patrolPath);
  }

  const Point& target = patrolPath[m_waypointIdx];

  double speed = 50.0 / m_frameRate;
  Vec2f v = normalise(target - body.pos) * speed;
  body.angle = atan2(v.y, v.x);

  spatialSystem.moveEntity(entityId(), v);

  if (distance(body.pos, target) < 10) {
    m_waypointIdx = (m_waypointIdx + 1) % patrolPath.size();
  }
}

//===========================================
// CEnemyBehaviour::doChasingBehaviour
//===========================================
void CEnemyBehaviour::doChasingBehaviour(SpatialSystem& spatialSystem, CVRect& body) {
  const Point& target = spatialSystem.sg.player->pos();

  double speed = 50.0 / m_frameRate;
  Vec2f v = normalise(target - body.pos) * speed;
  body.angle = atan2(v.y, v.x);

  spatialSystem.moveEntity(entityId(), v);
}

//===========================================
// CEnemyBehaviour::attemptShot
//===========================================
void CEnemyBehaviour::attemptShot(SpatialSystem& spatialSystem, const CVRect& body) {
  DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
  const Player& player = *spatialSystem.sg.player;

  m_gunfireTiming->doIfReady([&]() {
    if (hasLineOfSight(spatialSystem, body, player)) {
      DBG_PRINT("Bang!\n");
      m_audioManager.playSoundAtPos("shotgun_shoot", body.pos);
      damageSystem.damageEntity(player.body.entityId(), 1);
    }
  });
}

//===========================================
// CEnemyBehaviour::update
//===========================================
void CEnemyBehaviour::update() {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  CVRect& body = dynamic_cast<CVRect&>(spatialSystem.getComponent(entityId()));

  if (m_state == ST_PATROLLING && patrolPath.size() > 0) {
    doPatrollingBehaviour(spatialSystem, body);
  }
  else if (m_state == ST_CHASING) {
    doChasingBehaviour(spatialSystem, body);
  }

  attemptShot(spatialSystem, body);
}

//===========================================
// CEnemyBehaviour::handleEvent
//===========================================
void CEnemyBehaviour::handleEvent(const GameEvent& event) {
  if (event.name == "entityChangedZone") {
    SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
    const Player& player = *spatialSystem.sg.player;
    const EChangedZone& e = dynamic_cast<const EChangedZone&>(event);

    if (e.entityId == player.body.entityId()) {
      if (e.newZone == stPatrollingTrigger && m_state == ST_IDLE) {
        m_state = ST_PATROLLING;
      }
      else if (e.newZone == stChasingTrigger) {
        m_state = ST_CHASING;
      }
    }
  }
}
