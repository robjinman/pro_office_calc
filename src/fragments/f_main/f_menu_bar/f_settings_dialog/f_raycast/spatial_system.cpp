#include <cassert>
#include <ostream>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "exception.hpp"


using std::ostream;
using std::set;


ostream& operator<<(ostream& os, CSpatialKind kind) {
  switch (kind) {
    case CSpatialKind::ZONE: os << "ZONE"; break;
    case CSpatialKind::HARD_EDGE: os << "HARD_EDGE"; break;
    case CSpatialKind::SOFT_EDGE: os << "SOFT_EDGE"; break;
    case CSpatialKind::H_RECT: os << "H_RECT"; break;
    case CSpatialKind::V_RECT: os << "V_RECT"; break;
  }
  return os;
}

//===========================================
// addToZone
//===========================================
static void addToZone(CZone& zone, pCSpatial_t child) {
  switch (child->kind) {
    case CSpatialKind::ZONE: {
      pCZone_t ptr(dynamic_cast<CZone*>(child.release()));
      ptr->parent = &zone;
      zone.children.push_back(std::move(ptr));
      break;
    }
    case CSpatialKind::SOFT_EDGE:
    case CSpatialKind::HARD_EDGE: {
      pCEdge_t ptr(dynamic_cast<CEdge*>(child.release()));
      zone.edges.push_back(std::move(ptr));
      break;
    }
    case CSpatialKind::H_RECT: {
      pCHRect_t ptr(dynamic_cast<CHRect*>(child.release()));
      zone.hRects.push_back(std::move(ptr));
      break;
    }
    case CSpatialKind::V_RECT: {
      pCVRect_t ptr(dynamic_cast<CVRect*>(child.release()));
      zone.vRects.push_back(std::move(ptr));
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to zone");
  }
}

//===========================================
// addToHardEdge
//===========================================
static void addToHardEdge(CHardEdge& edge, pCSpatial_t child) {
  switch (child->kind) {
    case CSpatialKind::V_RECT: {
      pCVRect_t ptr(dynamic_cast<CVRect*>(child.release()));
      edge.vRects.push_back(std::move(ptr));
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to HardEdge");
  }
}

//===========================================
// addChildToComponent
//===========================================
static void addChildToComponent(CSpatial& parent, pCSpatial_t child) {
  switch (parent.kind) {
    case CSpatialKind::ZONE:
      addToZone(dynamic_cast<CZone&>(parent), std::move(child));
      break;
    case CSpatialKind::HARD_EDGE:
      addToHardEdge(dynamic_cast<CHardEdge&>(parent), std::move(child));
      break;
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to component of kind "
        << parent.kind);
  };
}

//===========================================
// removeFromZone
//===========================================
static void removeFromZone(CZone& zone, const CSpatial& child) {
  switch (child.kind) {
    case CSpatialKind::ZONE: {
      zone.children.remove_if([&](const pCZone_t& e) {
        return e.get() == dynamic_cast<const CZone*>(&child);
      });
      break;
    }
    case CSpatialKind::SOFT_EDGE:
    case CSpatialKind::HARD_EDGE: {
      zone.edges.remove_if([&](const pCEdge_t& e) {
        return e.get() == dynamic_cast<const CEdge*>(&child);
      });
      break;
    }
    case CSpatialKind::H_RECT: {
      zone.hRects.remove_if([&](const pCHRect_t& e) {
        return e.get() == dynamic_cast<const CHRect*>(&child);
      });
      break;
    }
    case CSpatialKind::V_RECT: {
      zone.vRects.remove_if([&](const pCVRect_t& e) {
        return e.get() == dynamic_cast<const CVRect*>(&child);
      });
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child.kind << " to zone");
  }
}

//===========================================
// removeFromHardEdge
//===========================================
static void removeFromHardEdge(CHardEdge& edge, const CSpatial& child) {
  switch (child.kind) {
    case CSpatialKind::V_RECT: {
      edge.vRects.remove_if([&](const pCVRect_t& e) {
        return e.get() == dynamic_cast<const CVRect*>(&child);
      });
      break;
    }
    default:
      EXCEPTION("Cannot remove component of kind " << child.kind << " from HardEdge");
  }
}

//===========================================
// removeChildFromComponent
//===========================================
static void removeChildFromComponent(CSpatial& parent, const CSpatial& child) {
  switch (parent.kind) {
    case CSpatialKind::ZONE:
      removeFromZone(dynamic_cast<CZone&>(parent), child);
      break;
    case CSpatialKind::HARD_EDGE:
      removeFromHardEdge(dynamic_cast<CHardEdge&>(parent), child);
      break;
    default:
      EXCEPTION("Cannot remove component of kind " << child.kind << " from component of kind "
        << parent.kind);
  };
}

//===========================================
// SpatialSystem::update
//===========================================
void SpatialSystem::update() {

}

//===========================================
// SpatialSystem::handleEvent
//===========================================
void SpatialSystem::handleEvent(const GameEvent& event) {

}

//===========================================
// SpatialSystem::addComponent
//===========================================
void SpatialSystem::addComponent(pComponent_t component) {
  if (component->kind() != ComponentKind::C_SPATIAL) {
    EXCEPTION("Component is not of kind C_SPATIAL");
  }

  CSpatial* ptr = dynamic_cast<CSpatial*>(component.release());
  pCSpatial_t c(ptr);

  if (c->parent == -1) {
    if (m_rootZone) {
      EXCEPTION("Root zone already set");
    }

    if (c->kind != CSpatialKind::ZONE) {
      EXCEPTION("Component has no parent; Only zones can be root");
    }

    pCZone_t z(dynamic_cast<CZone*>(c.release()));

    m_rootZone = std::move(z);
    m_components.clear();
  }
  else {
    auto it = m_components.find(c->parent);
    if (it == m_components.end()) {
      EXCEPTION("Could not find parent component with id " << c->parent);
    }

    CSpatial* parent = it->second;
    assert(parent->entityId() == c->parent);

    m_entityChildren[c->parent].insert(c->entityId());
    addChildToComponent(*parent, std::move(c));
  }

  m_components.insert(std::make_pair(c->entityId(), ptr));
}

//===========================================
// SpatialSystem::isRoot
//===========================================
bool SpatialSystem::isRoot(const CSpatial& c) const {
  if (c.kind != CSpatialKind::ZONE) {
    return false;
  }
  if (m_rootZone == nullptr) {
    return false;
  }
  const CZone* ptr = dynamic_cast<const CZone*>(&c);
  return ptr == m_rootZone.get();
}

//===========================================
// SpatialSystem::removeEntity_r
//===========================================
void SpatialSystem::removeEntity_r(entityId_t id) {
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
// SpatialSystem::removeEntity
//===========================================
void SpatialSystem::removeEntity(entityId_t id) {
  auto it = m_components.find(id);
  if (it == m_components.end()) {
    return;
  }

  CSpatial& c = *it->second;
  auto jt = m_components.find(c.parent);

  if (jt != m_components.end()) {
    CSpatial& parent = *jt->second;
    removeChildFromComponent(parent, c);
  }
  else {
    assert(isRoot(c));
  }

  removeEntity_r(id);
}
