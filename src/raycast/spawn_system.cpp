#include "raycast/spawn_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/root_factory.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/agent_system.hpp"
#include "exception.hpp"
#include "utils.hpp"


//===========================================
// CSpawn::CSpawn
//===========================================
CSpawn::~CSpawn() {}

//===========================================
// SpawnSystem::handleEvent
//===========================================
void SpawnSystem::handleEvent(const GameEvent& event) {
  if (event.name == "entityDestroyed") {
    const EEntityDestroyed& e = dynamic_cast<const EEntityDestroyed&>(event);

    auto it = m_spawnables.find(e.entityId);

    if (it != m_spawnables.end()) {
      const CSpawnable& spawnable = *it->second;

      entityId_t entityId = Component::getNextId();

      m_rootFactory.constructObject(spawnable.typeName, entityId, *spawnable.object,
        spawnable.parentId, spawnable.parentTransform);

      SpatialSystem& spatialSystem =
        m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

      const CSpatial& spatial = dynamic_cast<const CSpatial&>(spatialSystem.getComponent(entityId));

      // Only deal with V_RECTs for now
      if (spatial.kind == CSpatialKind::V_RECT) {
        const CVRect& vRect = dynamic_cast<const CVRect&>(spatial);

        CVRect& spawnPointVRect = m_entityManager.getComponent<CVRect>(spawnable.spawnPoint,
          ComponentKind::C_SPATIAL);

        Point oldPos = vRect.pos;
        spatialSystem.relocateEntity(entityId, *spawnPointVRect.zone, spawnPointVRect.pos);

        AgentSystem& agentSystem =
          m_entityManager.system<AgentSystem>(ComponentKind::C_AGENT);

        agentSystem.navigateTo(entityId, oldPos);
      }
    }
  }
}

//===========================================
// SpawnSystem::addComponent
//===========================================
void SpawnSystem::addComponent(pComponent_t component) {
  CSpawn& spawn = dynamic_cast<CSpawn&>(*component);

  switch (spawn.kind) {
    case CSpawnKind::SPAWN_POINT: {
      pCSpawnPoint_t c(dynamic_cast<CSpawnPoint*>(component.release()));
      m_spawnPoints.insert(make_pair(c->entityId(), std::move(c)));

      break;
    }
    case CSpawnKind::SPAWNABLE: {
      pCSpawnable_t c(dynamic_cast<CSpawnable*>(component.release()));
      m_spawnables.insert(make_pair(c->entityId(), std::move(c)));

      break;
    }
  }
}

//===========================================
// SpawnSystem::hasComponent
//===========================================
bool SpawnSystem::hasComponent(entityId_t entityId) const {
  return m_spawnPoints.count(entityId) || m_spawnables.count(entityId);
}

//===========================================
// SpawnSystem::getComponent
//===========================================
Component& SpawnSystem::getComponent(entityId_t entityId) const {
  auto it = m_spawnPoints.find(entityId);
  if (it != m_spawnPoints.end()) {
    return *it->second;
  }

  auto jt = m_spawnables.find(entityId);
  if (jt != m_spawnables.end()) {
    return *jt->second;
  }

  EXCEPTION("Component not found");
}

//===========================================
// SpawnSystem::removeEntity
//===========================================
void SpawnSystem::removeEntity(entityId_t id) {
  m_spawnPoints.erase(id);
  m_spawnables.erase(id);
}
