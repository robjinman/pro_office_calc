#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/c_enemy_behaviour.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/damage_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_components.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
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
// CEnemyBehaviour::CEnemyBehaviour
//===========================================
CEnemyBehaviour::CEnemyBehaviour(entityId_t entityId, EntityManager& entityManager,
  double frameRate)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_frameRate(frameRate) {

  m_gunfireTiming.reset(new TRandomIntervals(300, 7000));
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
  const Point& target = player.pos();
  Vec2f v = target - body.pos;
  Vec2f dv = normalise(v) * 0.0001;
  double hAngle = atan2(v.y, v.x);
  double vAngle = 0; // TODO
  Matrix m; // TODO

  list<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(*body.zone, body.pos + dv,
    hAngle, vAngle, m);

  if (intersections.front()->entityId == player.body.entityId()) {
    m_gunfireTiming->doIfReady([&]() {
      DBG_PRINT("Bang!\n");
      damageSystem.damageEntity(player.body.entityId(), 1);
    });
  }
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
