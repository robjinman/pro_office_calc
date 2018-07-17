#include <cassert>
#include "raycast/focus_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/time_service.hpp"
#include "exception.hpp"
#include "app_config.hpp"
#include "utils.hpp"


using std::vector;


//===========================================
// FocusSystem::FocusSystem
//===========================================
FocusSystem::FocusSystem(const AppConfig& appConfig, EntityManager& entityManager,
  TimeService& timeService)
  : m_appConfig(appConfig),
    m_entityManager(entityManager),
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
// isWallDecal
//===========================================
static bool isWallDecal(const RenderSystem& renderSystem, const Intersection& X) {
  if (renderSystem.hasComponent(X.entityId)) {
    const CRender& c = renderSystem.getComponent(X.entityId);
    return c.kind == CRenderKind::WALL_DECAL;
  }

  return false;
}

//===========================================
// getZIndex
//===========================================
static int getZIndex(const RenderSystem& renderSystem, const Intersection& X) {
  const CWallDecal& decal = DYNAMIC_CAST<const CWallDecal&>(renderSystem.getComponent(X.entityId));
  return decal.zIndex;
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
    auto first = intersections.begin();

    int highestZ = -9999;
    for (auto it = intersections.begin(); it != intersections.end(); ++it) {
      if (!isWallDecal(renderSystem, **it)) {
        break;
      }

      int z = getZIndex(renderSystem, **it);
      if (z > highestZ) {
        highestZ = z;
        first = it;
      }
    }

    pIntersection_t& X = *first;

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
  const Size& vp_wld = renderSystem.rg.viewport;
  const Size& vp_px = renderSystem.rg.viewport_px;

  double pxH = vp_wld.y / vp_px.y;
  double pxW = vp_wld.x / vp_px.x;

  double chH = 0.5;
  double chH_px = chH / pxH;

  QFont font = m_appConfig.monoFont;
  font.setPixelSize(chH_px);

  double margin = 0.2;

  QFontMetrics fm(font);
  double textW_px = fm.size(Qt::TextSingleLine, c.captionText.c_str()).width();
  double textW = textW_px * pxW;

  Size sz(2.0 * margin + textW, 2.0 * margin + chH);
  Point bgPos(0.5 * (vp_wld.x - sz.x), 0.70 * (vp_wld.y - sz.y));
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
