#include <cassert>
#include <vector>
#include <random>
#include "raycast/entity_manager.hpp"
#include "raycast/agent_system.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/time_service.hpp"
#include "raycast/audio_service.hpp"
#include "utils.hpp"
#include "exception.hpp"


using std::vector;
using std::function;


static const double AGENT_SPEED = 120.0;
static const double REACTION_SPEED = 0.25;


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
// CAgent::setAnimation
//===========================================
void CAgent::setAnimation(EntityManager& entityManager) {
  AnimationSystem& animationSystem =
    entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);

  switch (m_state) {
    case ST_STATIONARY:
      animationSystem.playAnimation(entityId(), "idle", true);
      break;
    case ST_SHOOTING:
      animationSystem.playAnimation(entityId(), "shoot", false);
      break;
    case ST_CHASING_OBJECT:
    case ST_ON_FIXED_PATH:
      animationSystem.playAnimation(entityId(), "run", true);
      break;
  }
}

//===========================================
// CAgent::setState
//===========================================
void CAgent::setState(EntityManager& entityManager, state_t state) {
  if (m_state != state) {
    m_state = state;
    setAnimation(entityManager);
  }
}

//===========================================
// CAgent::hasLineOfSight
//===========================================
bool CAgent::hasLineOfSight(const SpatialSystem& spatialSystem, Matrix& m, Vec2f& ray,
  double& hAngle, double& vAngle, double& height) const {

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

  vector<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(*body.zone, ray * 0.1,
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
void CAgent::startPatrol(EntityManager& entityManager) {
  if (patrolPath != -1) {
    const CPath& path = entityManager.getComponent<CPath>(patrolPath, ComponentKind::C_SPATIAL);

    m_path = path.points;
    m_pathClosed = true;
    setState(entityManager, ST_ON_FIXED_PATH);
    m_waypointIdx = -1;
  }
  else {
    setState(entityManager, ST_STATIONARY);
  }
}

//===========================================
// CAgent::startChase
//===========================================
void CAgent::startChase(EntityManager& entityManager) {
  SpatialSystem& spatialSystem = entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  m_waypointIdx = -1;
  setState(entityManager, ST_CHASING_OBJECT);
  m_targetObject = spatialSystem.sg.player->body;
}

//===========================================
// CAgent::followPath
//===========================================
void CAgent::followPath(EntityManager& entityManager, TimeService& timeService) {
  SpatialSystem& spatialSystem = entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  CVRect& body = dynamic_cast<CVRect&>(spatialSystem.getComponent(entityId()));

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
      setState(entityManager, ST_STATIONARY);

      if (m_onFinish) {
        m_onFinish(*this);
      }
    }
  }
}

//===========================================
// CAgent::attemptShot
//===========================================
void CAgent::attemptShot(EntityManager& entityManager, TimeService& timeService,
  AudioService& audioService) {

  AgentSystem& agentSystem = entityManager.system<AgentSystem>(ComponentKind::C_AGENT);
  SpatialSystem& spatialSystem = entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  DamageSystem& damageSystem = entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);

  CVRect& body = dynamic_cast<CVRect&>(spatialSystem.getComponent(entityId()));

  m_gunfireTiming->doIfReady([&]() {
    double hAngle = 0;
    double vAngle = 0;
    Matrix m;
    Vec2f ray;
    double height = 0;

    if (hasLineOfSight(spatialSystem, m, ray, hAngle, vAngle, height)) {
      body.angle = hAngle;
      entityId_t id = entityId();
      state_t prevState = m_state;
      setState(entityManager, ST_SHOOTING);

      timeService.onTimeout([&agentSystem, &entityManager, id, prevState, this]() {
        // If the entity still exists, our pointer is still good
        if (agentSystem.hasComponent(id)) {
          setState(entityManager, prevState);
        }
      }, 1.0);

      // Move ray randomly to simulate inaccurate aim
      //

      std::normal_distribution<double> dist(0, DEG_TO_RAD(1));

      double vDa = dist(randEngine);
      double hDa = dist(randEngine);

      Matrix rot(hDa, Vec2f(0, 0));
      ray = rot * ray;

      timeService.onTimeout([=, &agentSystem, &damageSystem, &audioService, &body]() {
        if (agentSystem.hasComponent(id)) {
          damageSystem.damageAtIntersection(*body.zone, ray * 0.1, height, ray, vAngle + vDa, m, 1);
          audioService.playSoundAtPos("shotgun_shoot", body.pos);
        }
      }, REACTION_SPEED);
    }
  });
}

//===========================================
// CAgent::navigateTo
//===========================================
void CAgent::navigateTo(EntityManager& entityManager, const Point& p,
  function<void(CAgent&)> onFinish) {

  SpatialSystem& spatialSystem = entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  AnimationSystem& animationSystem =
    entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);

  const CVRect& body = dynamic_cast<const CVRect&>(spatialSystem.getComponent(entityId()));

  m_path = spatialSystem.shortestPath(body.pos, p, 10);
  m_pathClosed = false;
  m_waypointIdx = -1;
  setState(entityManager, ST_ON_FIXED_PATH);
  m_onFinish = onFinish;

  animationSystem.playAnimation(entityId(), "run", true);
}

//===========================================
// CAgent::update
//===========================================
void CAgent::update(EntityManager& entityManager, TimeService& timeService,
  AudioService& audioService) {

  SpatialSystem& spatialSystem = entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
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
      followPath(entityManager, timeService);
      break;
    case ST_SHOOTING:
      break;
  }

  if (isHostile && player.alive) {
    attemptShot(entityManager, timeService, audioService);
  }
}

//===========================================
// CAgent::handleEvent
//===========================================
void CAgent::handleEvent(const GameEvent& event, EntityManager& entityManager) {
  if (event.name == stPatrollingTrigger) {
    startPatrol(entityManager);
  }
  else if (event.name == stChasingTrigger && m_state == ST_STATIONARY) {
    startChase(entityManager);
  }
  else if (event.name == "entityDamaged") {
    const EEntityDamaged& e = dynamic_cast<const EEntityDamaged&>(event);

    if (e.entityId == entityId()) {
      onDamage(entityManager);
    }
  }
}

//===========================================
// CAgent::onDamage
//===========================================
void CAgent::onDamage(EntityManager& entityManager) {
  // TODO: Chase player?
}

//===========================================
// AgentSystem::update
//===========================================
void AgentSystem::update() {
  for (auto& c : m_components) {
    c.second->update(m_entityManager, m_timeService, m_audioService);
  }
}

//===========================================
// AgentSystem::handleEvent
//===========================================
void AgentSystem::handleEvent(const GameEvent& event) {
  for (auto& c : m_components) {
    c.second->handleEvent(event, m_entityManager);
  }
}

//===========================================
// AgentSystem::navigateTo
//===========================================
void AgentSystem::navigateTo(entityId_t entityId, const Point& point) {
  DBG_PRINT("Entity " << entityId << " navigating to " << point << "\n");

  m_components.at(entityId)->navigateTo(m_entityManager, point, [this](CAgent& agent) {
    agent.startPatrol(m_entityManager);
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
CAgent& AgentSystem::getComponent(entityId_t entityId) const {
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
