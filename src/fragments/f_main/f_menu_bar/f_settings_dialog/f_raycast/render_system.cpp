#include <cassert>
#include <list>
#include <set>
#include <ostream>
#include <QImage>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "exception.hpp"


using std::string;
using std::list;
using std::set;
using std::vector;
using std::array;
using std::unique_ptr;
using std::ostream;
using std::function;


ostream& operator<<(ostream& os, CRenderKind kind) {
  switch (kind) {
    case CRenderKind::REGION: os << "REGION"; break;
    case CRenderKind::WALL: os << "WALL"; break;
    case CRenderKind::JOIN: os << "JOIN"; break;
    case CRenderKind::FLOOR_DECAL: os << "FLOOR_DECAL"; break;
    case CRenderKind::WALL_DECAL: os << "WALL_DECAL"; break;
    case CRenderKind::SPRITE: os << "SPRITE"; break;
    case CRenderKind::OVERLAY: os << "OVERLAY"; break;
  }
  return os;
}

static inline CSoftEdge& getSoftEdge(const SpatialSystem& spatialSystem, const CBoundary& b) {
  return dynamic_cast<CSoftEdge&>(spatialSystem.getComponent(b.entityId()));
}

//===========================================
// RenderSystem::RenderSystem
//===========================================
RenderSystem::RenderSystem(EntityManager& entityManager, QImage& target)
  : m_entityManager(entityManager),
    m_target(target),
    m_renderer(entityManager) {}

//===========================================
// connectSubregions_r
//===========================================
static void connectSubregions_r(const SpatialSystem& spatialSystem, CRegion& region) {
  for (auto it = region.children.begin(); it != region.children.end(); ++it) {
    CRegion& r = **it;
    connectSubregions_r(spatialSystem, r);

    for (auto jt = r.boundaries.begin(); jt != r.boundaries.end(); ++jt) {
      if ((*jt)->kind == CRenderKind::JOIN) {
        CJoin* je = dynamic_cast<CJoin*>(*jt);
        assert(je != nullptr);

        bool hasTwin = false;
        forEachCRegion(region, [&](CRegion& r_) {
          if (!hasTwin) {
            if (&r_ != &r) {
              for (auto lt = r_.boundaries.begin(); lt != r_.boundaries.end(); ++lt) {
                if ((*lt)->kind == CRenderKind::JOIN) {
                  CJoin* other = dynamic_cast<CJoin*>(*lt);

                  entityId_t id1 = getSoftEdge(spatialSystem, *je).joinId;
                  entityId_t id2 = getSoftEdge(spatialSystem, *other).joinId;

                  if (id1 == id2) {
                    hasTwin = true;

                    je->joinId = other->joinId;
                    je->regionA = other->regionA = &r;
                    je->regionB = other->regionB = &r_;

                    je->mergeIn(*other);
                    other->mergeIn(*je);

                    break;
                  }
                }
              }
            }
          }
        });

        if (!hasTwin) {
          je->regionA = &r;
          je->regionB = &region;
        }
      }
    }
  };
}

//===========================================
// RenderSystem::connectRegions
//
// Assumes SpatialSystem::connectZones() has already been called
//===========================================
void RenderSystem::connectRegions() {
  const SpatialSystem& spatialSystem = m_entityManager
    .system<SpatialSystem>(ComponentKind::C_SPATIAL);

  connectSubregions_r(spatialSystem, *rg.rootRegion);
}

//===========================================
// RenderSystem::render
//===========================================
void RenderSystem::render() {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  const Player& player = *spatialSystem.sg.player;
  const CRegion& currentRegion = dynamic_cast<const CRegion&>(*m_components
    .at(player.currentRegion));

  m_renderer.renderScene(m_target, rg, player, currentRegion);
}

//===========================================
// addToRegion
//===========================================
static void addToRegion(RenderGraph& rg, CRegion& region, pCRender_t child) {
  switch (child->kind) {
    case CRenderKind::REGION: {
      pCRegion_t ptr(dynamic_cast<CRegion*>(child.release()));
      region.children.push_back(std::move(ptr));
      break;
    }
    case CRenderKind::JOIN:
    case CRenderKind::WALL: {
      pCBoundary_t ptr(dynamic_cast<CBoundary*>(child.release()));
      region.boundaries.push_back(ptr.get());
      rg.boundaries.push_back(std::move(ptr));
      break;
    }
    case CRenderKind::FLOOR_DECAL: {
      pCFloorDecal_t ptr(dynamic_cast<CFloorDecal*>(child.release()));
      region.floorDecals.push_back(std::move(ptr));
      break;
    }
    case CRenderKind::SPRITE: {
      pCSprite_t ptr(dynamic_cast<CSprite*>(child.release()));
      region.sprites.push_back(std::move(ptr));
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to region");
  }
}

//===========================================
// addToWall
//===========================================
static void addToWall(CWall& boundary, pCRender_t child) {
  switch (child->kind) {
    case CRenderKind::WALL_DECAL: {
      pCWallDecal_t ptr(dynamic_cast<CWallDecal*>(child.release()));
      boundary.decals.push_back(std::move(ptr));
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to Wall");
  }
}

//===========================================
// addChildToComponent
//===========================================
static void addChildToComponent(RenderGraph& rg, CRender& parent, pCRender_t child) {
  switch (parent.kind) {
    case CRenderKind::REGION:
      addToRegion(rg, dynamic_cast<CRegion&>(parent), std::move(child));
      break;
    case CRenderKind::WALL:
      addToWall(dynamic_cast<CWall&>(parent), std::move(child));
      break;
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to component of kind "
        << parent.kind);
  };
}

//===========================================
// removeFromRegion
//===========================================
static void removeFromRegion(RenderGraph& rg, CRegion& region, const CRender& child) {
  switch (child.kind) {
    case CRenderKind::REGION: {
      auto it = find_if(region.children.begin(), region.children.end(), [&](const pCRegion_t& e) {
        return e.get() == dynamic_cast<const CRegion*>(&child);
      });
      region.children.erase(it);
      break;
    }
    case CRenderKind::JOIN:
    case CRenderKind::WALL: {
      auto it = find_if(region.boundaries.begin(), region.boundaries.end(),
        [&](const CBoundary* b) {

        return b == dynamic_cast<const CBoundary*>(&child);
      });
      region.boundaries.erase(it);

      auto jt = remove_if(rg.boundaries.begin(), rg.boundaries.end(), [&](const pCBoundary_t& b) {
        return b.get() == dynamic_cast<const CBoundary*>(&child);
      });
      rg.boundaries.erase(jt);
      break;
    }
    case CRenderKind::FLOOR_DECAL: {
      auto it = find_if(region.floorDecals.begin(), region.floorDecals.end(), [&](const pCFloorDecal_t& e) {
        return e.get() == dynamic_cast<const CFloorDecal*>(&child);
      });
      region.floorDecals.erase(it);
      break;
    }
    case CRenderKind::SPRITE: {
      auto it = find_if(region.sprites.begin(), region.sprites.end(), [&](const pCSprite_t& e) {
        return e.get() == dynamic_cast<const CSprite*>(&child);
      });
      region.sprites.erase(it);
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child.kind << " to region");
  }
}

//===========================================
// removeFromWall
//===========================================
static void removeFromWall(CWall& boundary, const CRender& child) {
  switch (child.kind) {
    case CRenderKind::WALL_DECAL: {
      boundary.decals.remove_if([&](const pCWallDecal_t& e) {
        return e.get() == dynamic_cast<const CWallDecal*>(&child);
      });
      break;
    }
    default:
      EXCEPTION("Cannot remove component of kind " << child.kind << " from Wall");
  }
}

//===========================================
// removeChildFromComponent
//===========================================
static void removeChildFromComponent(RenderGraph& rg, CRender& parent,
  const CRender& child) {

  switch (parent.kind) {
    case CRenderKind::REGION:
      removeFromRegion(rg, dynamic_cast<CRegion&>(parent), child);
      break;
    case CRenderKind::WALL:
      removeFromWall(dynamic_cast<CWall&>(parent), child);
      break;
    default:
      EXCEPTION("Cannot remove component of kind " << child.kind << " from component of kind "
        << parent.kind);
  };
}

//===========================================
// RenderSystem::getComponent
//===========================================
Component& RenderSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// RenderSystem::addComponent
//===========================================
void RenderSystem::addComponent(pComponent_t component) {
  if (component->kind() != ComponentKind::C_RENDER) {
    EXCEPTION("Component is not of kind C_RENDER");
  }

  CRender* ptr = dynamic_cast<CRender*>(component.release());
  pCRender_t c(ptr);

  if (c->parentId == -1) {
    if (c->kind == CRenderKind::REGION) {
      if (rg.rootRegion) {
        EXCEPTION("Root region already set");
      }

      pCRegion_t z(dynamic_cast<CRegion*>(c.release()));

      rg.rootRegion = std::move(z);
      m_components.clear();
    }
    else if (c->kind == CRenderKind::OVERLAY) {
      pCOverlay_t z(dynamic_cast<COverlay*>(c.release()));
      rg.overlays.push_back(pCOverlay_t(std::move(z)));
    }
    else {
      EXCEPTION("Component has no parent and is not of type REGION or OVERLAY");
    }
  }
  else {
    auto it = m_components.find(c->parentId);
    if (it == m_components.end()) {
      EXCEPTION("Could not find parent component with id " << c->parentId);
    }

    CRender* parent = it->second;
    assert(parent->entityId() == c->parentId);

    m_entityChildren[c->parentId].insert(c->entityId());
    addChildToComponent(rg, *parent, std::move(c));
  }

  m_components.insert(std::make_pair(ptr->entityId(), ptr));
}

//===========================================
// RenderSystem::isRoot
//===========================================
bool RenderSystem::isRoot(const CRender& c) const {
  if (c.kind != CRenderKind::REGION) {
    return false;
  }
  if (rg.rootRegion == nullptr) {
    return false;
  }
  const CRegion* ptr = dynamic_cast<const CRegion*>(&c);
  return ptr == rg.rootRegion.get();
}

//===========================================
// RenderSystem::removeEntity_r
//===========================================
void RenderSystem::removeEntity_r(entityId_t id) {
  m_components.erase(id);

  auto it = m_entityChildren.find(id);
  if (it != m_entityChildren.end()) {
    set<entityId_t>& children = it->second;

    for (auto jt = children.begin(); jt != children.end(); ++jt) {
      removeEntity_r(*jt);
    }
  }

  m_entityChildren.erase(id);
}

//===========================================
// RenderSystem::removeEntity
//===========================================
void RenderSystem::removeEntity(entityId_t id) {
  auto it = m_components.find(id);
  if (it == m_components.end()) {
    return;
  }

  CRender& c = *it->second;
  auto jt = m_components.find(c.parentId);

  if (jt != m_components.end()) {
    CRender& parent = *jt->second;
    removeChildFromComponent(rg, parent, c);
  }
  else {
    assert(isRoot(c));
  }

  removeEntity_r(id);
}
