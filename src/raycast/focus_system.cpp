#include "raycast/focus_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::vector;


//===========================================
// FocusSystem::FocusSystem
//===========================================
FocusSystem::FocusSystem(EntityManager& entityManager)
  : m_entityManager(entityManager) {}

//===========================================
// FocusSystem::initialise
//===========================================
void FocusSystem::initialise() {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  m_tooltipId = Component::getNextId();
  CTextOverlay* tooltip = new CTextOverlay(m_tooltipId, "", Point(7.3, 4.8), 0.5, Qt::white, 1);

  renderSystem.addComponent(pComponent_t(tooltip));

  m_focusDistance = spatialSystem.sg.player->activationRadius;

  m_initialised = true;
}

//===========================================
// FocusSystem::update
//===========================================
void FocusSystem::update() {
  if (!m_initialised) {
    initialise();
  }

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  CTextOverlay& tooltip = dynamic_cast<CTextOverlay&>(renderSystem.getComponent(m_tooltipId));

  vector<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(Vec2f(1.0, 0), 0,
    m_focusDistance);

  if (intersections.size() > 0) {
    pIntersection_t& X = intersections.front();

    auto it = m_components.find(X->entityId);
    if (it != m_components.end()) {
      CFocus& c = *it->second;

      tooltip.text = c.tooltip;

      if (c.onFocus) {
        c.onFocus();
      }
    }
    else {
      tooltip.text = "";
    }
  }
  else {
    tooltip.text = "";
  }
}

//===========================================
// FocusSystem::addComponent
//===========================================
void FocusSystem::addComponent(pComponent_t component) {
  pCFocus_t c(dynamic_cast<CFocus*>(component.release()));
  m_components.insert(make_pair(c->entityId(), std::move(c)));
}

//===========================================
// FocusSystem::hasComponent
//===========================================
bool FocusSystem::hasComponent(entityId_t entityId) const {
  return m_components.find(entityId) != m_components.end();
}

//===========================================
// FocusSystem::getComponent
//===========================================
CFocus& FocusSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// FocusSystem::removeEntity
//===========================================
void FocusSystem::removeEntity(entityId_t id) {
  m_components.erase(id);
}
