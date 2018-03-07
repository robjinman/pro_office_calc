#include <cassert>
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
using std::function;


static const double AGENT_SPEED = 80.0;


static std::mt19937 randEngine(randomSeed());


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
// CAgent::hasLineOfSight
//===========================================
bool CAgent::hasLineOfSight(SpatialSystem& spatialSystem, Matrix& m, Vec2f& ray, double& hAngle,
  double& vAngle, double& height) const {

  CVRect& body = dynamic_cast<CVRect&>(spatialSystem.getComponent(entityId()));
  const Player& player = *spatialSystem.sg.player;

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
// CAgent::startPatrol
//===========================================
void CAgent::startPatrol() {
  if (patrolPath.size() > 0) {
    m_path = patrolPath;
    m_pathClosed = true;
    m_state = ST_ON_FIXED_PATH;
    m_waypointIdx = -1;
  }
  else {
    m_state = ST_STATIONARY;
  }
}

//===========================================
// CAgent::followPath
//===========================================
void CAgent::followPath(SpatialSystem& spatialSystem, TimeService& timeService) {
  CVRect& body = dynamic_cast<CVRect&>(spatialSystem.getComponent(entityId()));

  if (!m_shooting) {
    if (m_waypointIdx == -1) {
      m_waypointIdx = indexOfClosestPoint(body.pos, m_path);
    }

    const Point& target = m_path[m_waypointIdx];

    double speed = AGENT_SPEED / timeService.frameRate;
    Vec2f v = normalise(target - body.pos) * speed;
    body.angle = atan2(v.y, v.x);

    spatialSystem.moveEntity(entityId(), v);

    if (distance(body.pos, target) < 10) {
      if (m_pathClosed) {
        m_waypointIdx = (m_waypointIdx + 1) % m_path.size();
      }
      else {
        m_state = ST_STATIONARY;

        if (m_onFinish) {
          m_onFinish(*this);
        }
      }
    }
  }
}

//===========================================
// CAgent::attemptShot
//===========================================
void CAgent::attemptShot(AgentSystem& agentSystem, SpatialSystem& spatialSystem,
  DamageSystem& damageSystem, TimeService& timeService, AudioService& audioService) {

  CVRect& body = dynamic_cast<CVRect&>(spatialSystem.getComponent(entityId()));

  m_gunfireTiming->doIfReady([&]() {
    double hAngle = 0;
    double vAngle = 0;
    Matrix m;
    Vec2f ray;
    double height = 0;

    if (hasLineOfSight(spatialSystem, m, ray, hAngle, vAngle, height)) {
      body.angle = hAngle;
      m_shooting = true;
      entityId_t id = entityId();

      timeService.onTimeout([&agentSystem, id, this]() {
        // If the entity still exists, our pointer is still good
        if (agentSystem.hasComponent(id)) {
          m_shooting = false;
        }
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
// CAgent::navigateTo
//===========================================
void CAgent::navigateTo(SpatialSystem& spatialSystem, const Point& p,
  function<void(CAgent&)> onFinish) {

  const CVRect& body = dynamic_cast<const CVRect&>(spatialSystem.getComponent(entityId()));

  m_path = spatialSystem.shortestPath(body.pos, p, 10);
  m_pathClosed = false;
  m_waypointIdx = -1;
  m_state = ST_ON_FIXED_PATH;
  m_onFinish = onFinish;
}

//===========================================
// CAgent::update
//===========================================
void CAgent::update(AgentSystem& agentSystem, SpatialSystem& spatialSystem,
  DamageSystem& damageSystem, TimeService& timeService, AudioService& audioService) {

  const Player& player = *spatialSystem.sg.player;

  switch (m_state) {
    case ST_STATIONARY:
      break;
    case ST_CHASING_OBJECT:
      if (player.alive) {
        assert(m_targetObject != -1);
        m_path = spatialSystem.shortestPath(entityId(), m_targetObject, 10);
        m_pathClosed = false;
      }
    case ST_ON_FIXED_PATH:
      assert(m_path.size() > 0);
      followPath(spatialSystem, timeService);
      break;
  }

  if (player.alive) {
    //attemptShot(agentSystem, spatialSystem, damageSystem, timeService, audioService);
  }
}

//===========================================
// CAgent::onPlayerChangeZone
//===========================================
void CAgent::onPlayerChangeZone(entityId_t newZone, SpatialSystem& spatialSystem) {
  const Player& player = *spatialSystem.sg.player;

  if (newZone == stPatrollingTrigger && m_state == ST_STATIONARY) {
    m_path = patrolPath;
    m_pathClosed = true;
    m_waypointIdx = -1;
    m_state = ST_ON_FIXED_PATH;
  }
  else if (newZone == stChasingTrigger) {
    m_waypointIdx = -1;
    m_state = ST_CHASING_OBJECT;
    m_targetObject = player.body;
  }
}

//===========================================
// CAgent::onDamage
//===========================================
void CAgent::onDamage(SpatialSystem& spatialSystem) {
  //const Player& player = *spatialSystem.sg.player;

  //m_waypointIdx = -1;
  //m_state = ST_CHASING_OBJECT;
  //m_targetObject = player.body;
}

//===========================================
// AgentSystem::update
//===========================================
void AgentSystem::update() {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);

  for (auto& c : m_components) {
    c.second->update(*this, spatialSystem, damageSystem, m_timeService, m_audioService);
  }
}

//===========================================
// AgentSystem::handleEvent
//===========================================
void AgentSystem::handleEvent(const GameEvent& event) {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  if (event.name == "entityChangedZone") {
    const EChangedZone& e = dynamic_cast<const EChangedZone&>(event);
    const Player& player = *spatialSystem.sg.player;

    if (e.entityId == player.body) {
      for (auto& c : m_components) {
        c.second->onPlayerChangeZone(e.newZone, spatialSystem);
      }
    }
  }
  else if (event.name == "entityDamaged") {
    const EEntityDamaged& e = dynamic_cast<const EEntityDamaged&>(event);

    auto it = m_components.find(e.entityId);
    if (it != m_components.end()) {
      it->second->onDamage(spatialSystem);
    }
  }
}

//===========================================
// AgentSystem::navigateTo
//===========================================
void AgentSystem::navigateTo(entityId_t entityId, const Point& point) {
  DBG_PRINT("Entity " << entityId << " navigating to " << point << "\n");

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  m_components.at(entityId)->navigateTo(spatialSystem, point, [](CAgent& agent) {
    agent.startPatrol();
  });
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
