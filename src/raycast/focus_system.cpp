#include <cassert>
#include "raycast/focus_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/time_service.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::vector;


//===========================================
// FocusSystem::FocusSystem
//===========================================
FocusSystem::FocusSystem(EntityManager& entityManager, TimeService& timeService)
  : m_entityManager(entityManager),
    m_timeService(timeService) {}

//===========================================
// FocusSystem::initialise
//===========================================
void FocusSystem::initialise() {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  m_toolTipId = Component::getNextId();
  CTextOverlay* toolTip = new CTextOverlay(m_toolTipId, "", Point(7.3, 4.8), 0.5, Qt::white, 1);

  renderSystem.addComponent(pComponent_t(toolTip));

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

  CTextOverlay& toolTip = dynamic_cast<CTextOverlay&>(renderSystem.getComponent(m_toolTipId));
  toolTip.text = "";

  vector<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(Vec2f(1.0, 0), 0,
    m_focusDistance);

  if (intersections.size() > 0) {
    pIntersection_t& X = intersections.front();

    auto it = m_components.find(X->entityId);
    if (it != m_components.end()) {
      CFocus& c = *it->second;

      toolTip.text = c.hoverText;

      if (c.onHover) {
        c.onHover();
      }
    }
  }
}

//===========================================
// FocusSystem::deleteCaption
//===========================================
void FocusSystem::deleteCaption() {
  m_timeService.cancelTimeout(m_captionTimeoutId);
  m_entityManager.deleteEntity(m_captionBgId);
  m_entityManager.deleteEntity(m_captionTextId);

  m_captionBgId = -1;
  m_captionTextId = -1;
  m_captionTimeoutId = -1;
}

//===========================================
// FocusSystem::showCaption
//===========================================
void FocusSystem::showCaption(entityId_t entityId) {
  auto it = m_components.find(entityId);
  if (it == m_components.end()) {
    return;
  }

  const CFocus& c = *it->second;

  deleteCaption();

  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  const Size& vp = renderSystem.rg.viewport;

  double margin = 0.2;
  double chH = 0.6;
  double chW = 0.3;
  Size sz(2.0 * margin + chW * c.captionText.length(), 2.0 * margin + chH);
  Point bgPos(0.5 * (vp.x - sz.x), 0.75 * (vp.y - sz.y));
  Point textPos = bgPos + Vec2f(margin, margin);

  QColor bgColour(0, 0, 0, 120);
  QColor textColour(210, 210, 0);

  m_captionBgId = Component::getNextId();
  m_captionTextId = Component::getNextId();

  CColourOverlay* bgOverlay = new CColourOverlay(m_captionBgId, bgColour, bgPos, sz, 8);
  CTextOverlay* textOverlay = new CTextOverlay(m_captionTextId, c.captionText, textPos, chH,
    textColour, 9);

  renderSystem.addComponent(pComponent_t(bgOverlay));
  renderSystem.addComponent(pComponent_t(textOverlay));

  m_captionTimeoutId = m_timeService.onTimeout([this]() {
    deleteCaption();
  }, 3.0);
}

//===========================================
// FocusSystem::addComponent
//===========================================
void FocusSystem::addComponent(pComponent_t component) {
  CFocus* ptr = dynamic_cast<CFocus*>(component.release());
  m_components.insert(make_pair(ptr->entityId(), pCFocus_t(ptr)));
}

//===========================================
// FocusSystem::hasComponent
//===========================================
bool FocusSystem::hasComponent(entityId_t entityId) const {
  return m_components.count(entityId);
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