#include <list>
#include <random>
#include "raycast/agent_system.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/time_service.hpp"
#include "raycast/audio_service.hpp"
#include "utils.hpp"
#include "exception.hpp"


using std::vector;
using std::list;


static std::random_device rd;
static std::mt19937 randEngine(rd());


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
static bool hasLineOfSight(SpatialSystem& spatialSystem, const CVRect& body, const Player& player,
  Matrix& m, Vec2f& ray, double& hAngle, double& vAngle, double& height) {

  const Point& target_wld = player.pos();

  double targetHeight = player.feetHeight() + 0.5 * player.getTallness();
  Vec2f v = target_wld - body.pos;
  hAngle = atan2(v.y, v.x);

  Matrix t(hAngle, body.pos);
  m = t.inverse();

  Point target_rel = m * target_wld;
  ray = normalise(target_rel);

  height = body.zone->floorHeight + body.size.y * 0.5;
  vAngle = atan2(targetHeight - height, length(target_rel));

  list<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(*body.zone, ray * 0.1,
    height, ray, vAngle, m);

  if (intersections.size() > 0) {
    entityId_t id = intersections.front()->entityId;
    if (id == player.body) {
      return true;
    }
  }

  return false;
}

//===========================================
// CAgent::CAgent
//===========================================
CAgent::CAgent(entityId_t entityId)
  : Component(entityId, ComponentKind::C_AGENT) {

  m_gunfireTiming.reset(new TRandomIntervals(400, 4000));
}

//===========================================
// CAgent::doPatrollingBehaviour
//===========================================
void CAgent::doPatrollingBehaviour(SpatialSystem& spatialSystem, TimeService& timeService,
  CVRect& body) {

  if (!m_shooting) {
    if (m_waypointIdx == -1) {
      m_waypointIdx = indexOfClosestPoint(body.pos, patrolPath);
    }

    const Point& target = patrolPath[m_waypointIdx];

    double speed = 50.0 / timeService.frameRate;
    Vec2f v = normalise(target - body.pos) * speed;
    body.angle = atan2(v.y, v.x);

    spatialSystem.moveEntity(entityId(), v);

    if (distance(body.pos, target) < 10) {
      m_waypointIdx = (m_waypointIdx + 1) % patrolPath.size();
    }
  }
}

//===========================================
// CAgent::doChasingBehaviour
//===========================================
void CAgent::doChasingBehaviour(SpatialSystem& spatialSystem, TimeService& timeService,
  CVRect& body) {

  const Point& target = spatialSystem.sg.player->pos();

  double speed = 50.0 / timeService.frameRate;
  Vec2f v = normalise(target - body.pos) * speed;
  body.angle = atan2(v.y, v.x);

  if (!m_shooting) {
    spatialSystem.moveEntity(entityId(), v);
  }
}

//===========================================
// CAgent::attemptShot
//===========================================
void CAgent::attemptShot(SpatialSystem& spatialSystem, DamageSystem& damageSystem,
  TimeService& timeService, AudioService& audioService, CVRect& body) {

  const Player& player = *spatialSystem.sg.player;

  m_gunfireTiming->doIfReady([&]() {
    double hAngle = 0;
    double vAngle = 0;
    Matrix m;
    Vec2f ray;
    double height = 0;

    if (hasLineOfSight(spatialSystem, body, player, m, ray, hAngle, vAngle, height)) {
      body.angle = hAngle;
      m_shooting = true;

      timeService.onTimeout([&]() {
        m_shooting = false;
      }, 1.0);

      audioService.playSoundAtPos("shotgun_shoot", body.pos);

      // Move ray randomly to simulate inaccurate aim
      //

      std::normal_distribution<double> dist(0, DEG_TO_RAD(2));

      double vDa = dist(randEngine);
      double hDa = dist(randEngine);

      Matrix rot(hDa, Vec2f(0, 0));
      ray = rot * ray;

      damageSystem.damageAtIntersection(*body.zone, ray * 0.1, height, ray, vAngle + vDa, m, 1);
    }
  });
}

//===========================================
// CAgent::update
//===========================================
void CAgent::update(SpatialSystem& spatialSystem, DamageSystem& damageSystem,
  TimeService& timeService, AudioService& audioService) {

  CVRect& body = dynamic_cast<CVRect&>(spatialSystem.getComponent(entityId()));

  if (m_state == ST_PATROLLING && patrolPath.size() > 0) {
    doPatrollingBehaviour(spatialSystem, timeService, body);
  }
  else if (m_state == ST_CHASING) {
    doChasingBehaviour(spatialSystem, timeService, body);
  }

  attemptShot(spatialSystem, damageSystem, timeService, audioService, body);
}

//===========================================
// CAgent::handleEvent
//===========================================
void CAgent::handleEvent(const GameEvent& event, SpatialSystem& spatialSystem) {
  if (event.name == "entityChangedZone") {
    const Player& player = *spatialSystem.sg.player;
    const EChangedZone& e = dynamic_cast<const EChangedZone&>(event);

    if (e.entityId == player.body) {
      if (e.newZone == stPatrollingTrigger && m_state == ST_IDLE) {
        m_state = ST_PATROLLING;
      }
      else if (e.newZone == stChasingTrigger) {
        m_state = ST_CHASING;
      }
    }
  }
}

//===========================================
// AgentSystem::update
//===========================================
void AgentSystem::update() {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);

  for (auto& c : m_components) {
    c.second->update(spatialSystem, damageSystem, m_timeService, m_audioService);
  }
}

//===========================================
// AgentSystem::handleEvent
//===========================================
void AgentSystem::handleEvent(const GameEvent& event) {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  for (auto& c : m_components) {
    c.second->handleEvent(event, spatialSystem);
  }
}

//===========================================
// AgentSystem::navigateTo
//===========================================
void AgentSystem::navigateTo(entityId_t entityId, const Point& point) {
  DBG_PRINT("Entity " << entityId << " navigating to " << point << "\n");
}

//===========================================
// AgentSystem::hasComponent
//===========================================
bool AgentSystem::hasComponent(entityId_t entityId) const {
  return m_components.find(entityId) != m_components.end();
}

//===========================================
// AgentSystem::getComponent
//===========================================
Component& AgentSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// AgentSystem::addComponent
//===========================================
void AgentSystem::addComponent(pComponent_t component) {
  if (component->kind() != ComponentKind::C_AGENT) {
    EXCEPTION("Component is not of type CAgent");
  }

  CAgent* p = dynamic_cast<CAgent*>(component.release());
  m_components.insert(std::make_pair(p->entityId(), pCAgent_t(p)));
}

//===========================================
// AgentSystem::removeEntity
//===========================================
void AgentSystem::removeEntity(entityId_t entityId) {
  m_components.erase(entityId);
}
