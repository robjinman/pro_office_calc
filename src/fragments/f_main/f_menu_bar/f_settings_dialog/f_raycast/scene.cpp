#include <string>
#include <cassert>
#include <sstream>
#include <ostream>
#include <list>
#include <iterator>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_object_factory.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/tween_curves.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "exception.hpp"
#include "utils.hpp"
#include "event_system.hpp"


using std::stringstream;
using std::unique_ptr;
using std::function;
using std::string;
using std::list;
using std::map;
using std::set;
using std::ostream;


ostream& operator<<(ostream& os, CRenderSpatialKind kind) {
  switch (kind) {
    case CRenderSpatialKind::REGION: os << "REGION"; break;
    case CRenderSpatialKind::WALL: os << "WALL"; break;
    case CRenderSpatialKind::JOINING_EDGE: os << "JOINING_EDGE"; break;
    case CRenderSpatialKind::FLOOR_DECAL: os << "FLOOR_DECAL"; break;
    case CRenderSpatialKind::WALL_DECAL: os << "WALL_DECAL"; break;
    case CRenderSpatialKind::SPRITE: os << "SPRITE"; break;
  }
  return os;
}


//===========================================
// forEachConstRegion
//===========================================
void forEachConstRegion(const Region& region, function<void(const Region&)> fn) {
  fn(region);
  std::for_each(region.children.begin(), region.children.end(),
    [&](const unique_ptr<Region>& r) {

    forEachConstRegion(*r, fn);
  });
}

//===========================================
// forEachRegion
//===========================================
void forEachRegion(Region& region, function<void(Region&)> fn) {
  fn(region);
  std::for_each(region.children.begin(), region.children.end(),
    [&](unique_ptr<Region>& r) {

    forEachRegion(*r, fn);
  });
}

//===========================================
// getNextRegion
//===========================================
static Region* getNextRegion(const Region& current, const JoiningEdge& je) {
  return je.regionA == &current ? je.regionB : je.regionA;
}

//===========================================
// canStepAcross
//===========================================
static bool canStepAcross(const Player& player, const Region& currentRegion,
  const JoiningEdge& je) {

  Region* nextRegion = getNextRegion(currentRegion, je);

  bool canStep = nextRegion->floorHeight - player.feetHeight() <= PLAYER_STEP_HEIGHT;
  bool hasHeadroom = player.headHeight() < nextRegion->ceilingHeight;

  return canStep && hasHeadroom;
}

//===========================================
// intersectWall
//===========================================
static bool intersectWall(const Region& region, const Circle& circle, const Player& player) {
  bool b = false;

  forEachConstRegion(region, [&](const Region& r) {
    if (!b) {
      for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
        const Edge& edge = **it;

        if (edge.kind == CRenderSpatialKind::JOINING_EDGE) {
          const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

          //assert(&region == je.regionA || &region == je.regionB);
          if (&region != je.regionA && &region != je.regionB) {
            continue;
          }

          if (canStepAcross(player, region, je)) {
            continue;
          }
        }

        if (lineSegmentCircleIntersect(circle, edge.lseg)) {
          b = true;
          break;
        }
      }
    }
  });

  return b;
}

//===========================================
// getDelta
//
// Takes the vector the player wants to move in (dv) and returns a modified vector that doesn't
// allow the player within radius units of a wall.
//===========================================
static Vec2f getDelta(const Region& region, const Point& camPos, const Player& player,
  double radius, const Vec2f& dv) {

  Circle circle{camPos + dv, radius};
  LineSegment ray(camPos, camPos + dv);

  Vec2f dv_ = dv;

  assert(region.parent != nullptr);

  bool abortLoop = false;
  forEachConstRegion(*region.parent, [&](const Region& r) {
    if (abortLoop == false) {
      for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
        const Edge& edge = **it;

        // If moving by dv will intersect something
        if (lineSegmentCircleIntersect(circle, edge.lseg)) {
          Point p = lineIntersect(ray.line(), edge.lseg.line());
          // If we're moving away from the wall
          if (distance(camPos + dv_ * 0.00001, p) > distance(camPos, p)) {
            continue;
          }

          if (edge.kind == CRenderSpatialKind::JOINING_EDGE) {
            const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

            if (&region != je.regionA && &region != je.regionB) {
              continue;
            }

            if (canStepAcross(player, region, je)) {
              continue;
            }
          }

          // Erase component in direction of wall
          Matrix m(-atan(edge.lseg.line().m), Vec2f());
          dv_ = m * dv;
          dv_.y = 0;
          dv_ = m.inverse() * dv_;

          if (intersectWall(region, Circle{camPos + dv_, radius}, player)) {
            dv_ = Vec2f(0, 0);
          }
          else {
            abortLoop = true;
            break;
          }
        }
      }
    }
  });

  return dv_;
}

//===========================================
// playerBounce
//===========================================
static void playerBounce(Scene& scene) {
  double dy = 5.0;
  double frames = 20;
  double dy_ = dy / (frames / 2);
  int i = 0;
  scene.addTween(Tween{[&, dy_, i, frames]() mutable -> bool {
    if (i < frames / 2) {
      scene.sg.player->changeTallness(dy_);
    }
    else {
      scene.sg.player->changeTallness(-dy_);
    }
    return ++i < frames;
  }, []() {}}, "playerBounce");
}

//===========================================
// Scene::Scene
//===========================================
Scene::Scene(EntityManager& entityManager, double frameRate)
  : m_entityManager(entityManager) {

  m_frameRate = frameRate;
}

//===========================================
// Scene::loadMap
//===========================================
void Scene::loadMap(const string& mapFilePath) {
  list<parser::pObject_t> objects;
  parser::parse(mapFilePath, objects);

  assert(objects.size() == 1);
  constructRootRegion(m_entityManager, **objects.begin());
}

//===========================================
// Scene::handleEvent
//===========================================
void Scene::handleEvent(const GameEvent& event) {
  if (event.name == "playerActivate") {
    GameEvent e("activateEntity");
    e.entitiesInRange = getEntitiesInRadius(sg.player->activationRadius);

    m_entityManager.broadcastEvent(e);
  }
}

//===========================================
// Scene::~Scene
//===========================================
Scene::~Scene() {}

//===========================================
// Scene::vRotateCamera
//===========================================
void Scene::vRotateCamera(double da) {
  sg.player->vRotate(da);
}

//===========================================
// Scene::hRotateCamera
//===========================================
void Scene::hRotateCamera(double da) {
  sg.player->hRotate(da);
}

//===========================================
// Scene::translateCamera
//===========================================
void Scene::translateCamera(const Vec2f& dir) {
  const Camera& cam = sg.player->camera();

  Vec2f dv(cos(cam.angle) * dir.x - sin(cam.angle) * dir.y,
    sin(cam.angle) * dir.x + cos(cam.angle) * dir.y);

  double radius = 5.0;

  Region& currentRegion = getCurrentRegion();

  dv = getDelta(currentRegion, cam.pos, *sg.player, radius, dv);
  Circle circle{cam.pos + dv, radius};

  assert(currentRegion.parent != nullptr);

  bool abortLoop = false;
  forEachConstRegion(*currentRegion.parent, [&](const Region& region) {
    if (abortLoop) {
      return;
    }

    int nIntersections = 0;
    double nearestX = 999999.9;
    Region* nextRegion = nullptr;
    Point p;

    for (auto it = region.edges.begin(); it != region.edges.end(); ++it) {
      const Edge& edge = **it;

      if (lineSegmentCircleIntersect(circle, edge.lseg)) {
        assert(edge.kind == CRenderSpatialKind::JOINING_EDGE);
        const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

        LineSegment ray(cam.pos, cam.pos + dv);
        Point p_;
        bool crossesLine = lineSegmentIntersect(ray, edge.lseg, p_);

        if (crossesLine) {
          ++nIntersections;

          double dist = distance(cam.pos, p_);
          if (dist < nearestX) {
            nextRegion = getNextRegion(currentRegion, je);
            p = p_;

            nearestX = dist;
          }
        }
      }
    }

    if (nIntersections > 0) {
      sg.player->currentRegion = nextRegion->entityId();
      sg.player->setPosition(p);
      dv = dv * 0.00001;
      abortLoop = true;
    }
  });

  sg.player->move(dv);
  playerBounce(*this);
}

//===========================================
// Scene::jump
//===========================================
void Scene::jump() {
  if (!sg.player->aboveGround(getCurrentRegion())) {
    sg.player->vVelocity = 220;
  }
}

//===========================================
// Scene::addTween
//===========================================
void Scene::addTween(const Tween& tween, const char* name) {
  string s;
  if (name != nullptr) {
    s.assign(name);
  }
  else {
    stringstream ss;
    ss << "tween" << rand();
    s = ss.str();
  }

  if (m_tweens.find(s) == m_tweens.end()) {
    m_tweens[s] = tween;
  }
}

//===========================================
// Scene::gravity
//===========================================
void Scene::gravity() {
  Region& currentRegion = getCurrentRegion();

  if (fabs(sg.player->vVelocity) > 0.001 || sg.player->aboveGround(currentRegion)) {
    double a = -600.0;
    double dt = 1.0 / m_frameRate;
    double dv = dt * a;
    sg.player->vVelocity += dv;
    double dy = sg.player->vVelocity * dt;

    sg.player->changeHeight(currentRegion, dy);

    if (!sg.player->aboveGround(currentRegion)) {
      sg.player->vVelocity = 0;
    }
  }
}

//===========================================
// Scene::buoyancy
//===========================================
void Scene::buoyancy() {
  Region& currentRegion = getCurrentRegion();

  if (sg.player->feetHeight() + 0.1 < currentRegion.floorHeight) {
    double dy = 150.0 / m_frameRate;
    sg.player->changeHeight(currentRegion, dy);
  }
}

//===========================================
// overlapsCircle
//===========================================
static bool overlapsCircle(const Circle& circle, const Edge& edge) {
  return lineSegmentCircleIntersect(circle, edge.lseg); // TODO
}

//===========================================
// overlapsCircle
//===========================================
static bool overlapsCircle(const Circle& circle, const Sprite& sprite) {
  return distance(circle.pos, sprite.pos) <= circle.radius;
}

//===========================================
// overlapsCircle
//===========================================
static bool overlapsCircle(const Circle& circle, const FloorDecal& decal) {
  // TODO

  return false;
}

//===========================================
// getEntitiesInRadius
//===========================================
static void getEntitiesInRadius_r(const Region& region, const Circle& circle,
  set<entityId_t>& entities) {

  for (auto it = region.edges.begin(); it != region.edges.end(); ++it) {
    if (overlapsCircle(circle, **it)) {
      entities.insert((*it)->entityId());
      entities.insert(region.entityId());
    }
  }

  for (auto it = region.sprites.begin(); it != region.sprites.end(); ++it) {
    if (overlapsCircle(circle, **it)) {
      entities.insert((*it)->entityId());
    }
  }

  for (auto it = region.floorDecals.begin(); it != region.floorDecals.end(); ++it) {
    if (overlapsCircle(circle, **it)) {
      entities.insert((*it)->entityId());
    }
  }

  for (auto it = region.children.begin(); it != region.children.end(); ++it) {
    getEntitiesInRadius_r(**it, circle, entities);
  }
}

//===========================================
// similar
//===========================================
static bool similar(const LineSegment& l1, const LineSegment& l2) {
  double delta = 4.0;
  return (distance(l1.A, l2.A) <= delta && distance(l1.B, l2.B) <= delta)
    || (distance(l1.A, l2.B) <= delta && distance(l1.B, l2.A) <= delta);
}

//===========================================
// areTwins
//===========================================
static bool areTwins(const JoiningEdge& je1, const JoiningEdge& je2) {
  return similar(je1.lseg, je2.lseg);
}

//===========================================
// connectSubregions_r
//===========================================
static void connectSubregions_r(Region& region) {
  if (region.children.size() == 0) {
    return;
  }

  for (auto it = region.children.begin(); it != region.children.end(); ++it) {
    Region& r = **it;
    connectSubregions_r(r);

    for (auto jt = r.edges.begin(); jt != r.edges.end(); ++jt) {
      if ((*jt)->kind == CRenderSpatialKind::JOINING_EDGE) {
        JoiningEdge* je = dynamic_cast<JoiningEdge*>(*jt);
        assert(je != nullptr);

        bool hasTwin = false;
        forEachRegion(region, [&](Region& r_) {
          if (!hasTwin) {
            if (&r_ != &r) {
              for (auto lt = r_.edges.begin(); lt != r_.edges.end(); ++lt) {
                if ((*lt)->kind == CRenderSpatialKind::JOINING_EDGE) {
                  JoiningEdge* other = dynamic_cast<JoiningEdge*>(*lt);
                  assert(other != nullptr);

                  if (je == other) {
                    hasTwin = true;
                    break;
                  }

                  if (areTwins(*je, *other)) {
                    hasTwin = true;

                    je->joinId = other->joinId;
                    je->regionA = other->regionA = &r;
                    je->regionB = other->regionB = &r_;

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
// Scene::connectRegions
//===========================================
void Scene::connectRegions() {
  connectSubregions_r(*sg.rootRegion);
}

//===========================================
// Scene::getEntitiesInRadius
//===========================================
set<entityId_t> Scene::getEntitiesInRadius(double radius) const {
  set<entityId_t> entities;

  const Point& pos = sg.player->pos();
  Circle circle{pos, radius};

  forEachConstRegion(*getCurrentRegion().parent, [&](const Region& region) {
    getEntitiesInRadius_r(region, circle, entities);
  });

  return entities;
}

//===========================================
// Scene::update
//===========================================
void Scene::update() {
  for (auto it = m_tweens.begin(); it != m_tweens.end();) {
    const Tween& tween = it->second;

    if (!tween.tick()) {
      tween.finish();
      m_tweens.erase(it++);
    }
    else {
      ++it;
    }
  }

  buoyancy();
  gravity();
}

//===========================================
// addToRegion
//===========================================
static void addToRegion(SceneGraph& sg, Region& region, pCRenderSpatial_t child) {
  switch (child->kind) {
    case CRenderSpatialKind::REGION: {
      pRegion_t ptr(dynamic_cast<Region*>(child.release()));
      ptr->parent = &region;
      region.children.push_back(std::move(ptr));
      break;
    }
    case CRenderSpatialKind::JOINING_EDGE:
    case CRenderSpatialKind::WALL: {
      pEdge_t ptr(dynamic_cast<Edge*>(child.release()));
      region.edges.push_back(ptr.get());
      sg.edges.push_back(std::move(ptr));
      break;
    }
    case CRenderSpatialKind::FLOOR_DECAL: {
      pFloorDecal_t ptr(dynamic_cast<FloorDecal*>(child.release()));
      region.floorDecals.push_back(std::move(ptr));
      break;
    }
    case CRenderSpatialKind::SPRITE: {
      pSprite_t ptr(dynamic_cast<Sprite*>(child.release()));
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
static void addToWall(Wall& edge, pCRenderSpatial_t child) {
  switch (child->kind) {
    case CRenderSpatialKind::WALL_DECAL: {
      pWallDecal_t ptr(dynamic_cast<WallDecal*>(child.release()));
      edge.decals.push_back(std::move(ptr));
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to Wall");
  }
}

//===========================================
// addChildToComponent
//===========================================
static void addChildToComponent(SceneGraph& sg, CRenderSpatial& parent, pCRenderSpatial_t child) {
  switch (parent.kind) {
    case CRenderSpatialKind::REGION:
      addToRegion(sg, dynamic_cast<Region&>(parent), std::move(child));
      break;
    case CRenderSpatialKind::WALL:
      addToWall(dynamic_cast<Wall&>(parent), std::move(child));
      break;
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to component of kind "
        << parent.kind);
  };
}

//===========================================
// removeFromRegion
//===========================================
static void removeFromRegion(SceneGraph& sg, Region& region, const CRenderSpatial& child) {
  switch (child.kind) {
    case CRenderSpatialKind::REGION: {
      region.children.remove_if([&](const pRegion_t& e) {
        return e.get() == dynamic_cast<const Region*>(&child);
      });
      break;
    }
    case CRenderSpatialKind::JOINING_EDGE:
    case CRenderSpatialKind::WALL: {
      region.edges.remove_if([&](const Edge* e) {
        return e == dynamic_cast<const Edge*>(&child);
      });
      sg.edges.remove_if([&](const pEdge_t& e) {
        return e.get() == dynamic_cast<const Edge*>(&child);
      });
      break;
    }
    case CRenderSpatialKind::FLOOR_DECAL: {
      region.floorDecals.remove_if([&](const pFloorDecal_t& e) {
        return e.get() == dynamic_cast<const FloorDecal*>(&child);
      });
      break;
    }
    case CRenderSpatialKind::SPRITE: {
      region.sprites.remove_if([&](const pSprite_t& e) {
        return e.get() == dynamic_cast<const Sprite*>(&child);
      });
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child.kind << " to region");
  }
}

//===========================================
// removeFromWall
//===========================================
static void removeFromWall(Wall& edge, const CRenderSpatial& child) {
  switch (child.kind) {
    case CRenderSpatialKind::WALL_DECAL: {
      edge.decals.remove_if([&](const pWallDecal_t& e) {
        return e.get() == dynamic_cast<const WallDecal*>(&child);
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
static void removeChildFromComponent(SceneGraph& sg, CRenderSpatial& parent,
  const CRenderSpatial& child) {

  switch (parent.kind) {
    case CRenderSpatialKind::REGION:
      removeFromRegion(sg, dynamic_cast<Region&>(parent), child);
      break;
    case CRenderSpatialKind::WALL:
      removeFromWall(dynamic_cast<Wall&>(parent), child);
      break;
    default:
      EXCEPTION("Cannot remove component of kind " << child.kind << " from component of kind "
        << parent.kind);
  };
}

//===========================================
// Scene::getComponent
//===========================================
Component& Scene::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// Scene::addComponent
//===========================================
void Scene::addComponent(pComponent_t component) {
  if (component->kind() != ComponentKind::C_RENDER_SPATIAL) {
    EXCEPTION("Component is not of kind C_RENDER_SPATIAL");
  }

  CRenderSpatial* ptr = dynamic_cast<CRenderSpatial*>(component.release());
  pCRenderSpatial_t c(ptr);

  if (c->parentId == -1) {
    if (sg.rootRegion) {
      EXCEPTION("Root region already set");
    }

    if (c->kind != CRenderSpatialKind::REGION) {
      EXCEPTION("Component has no parent; Only regions can be root");
    }

    pRegion_t z(dynamic_cast<Region*>(c.release()));

    sg.rootRegion = std::move(z);
    m_components.clear();
  }
  else {
    auto it = m_components.find(c->parentId);
    if (it == m_components.end()) {
      EXCEPTION("Could not find parent component with id " << c->parentId);
    }

    CRenderSpatial* parent = it->second;
    assert(parent->entityId() == c->parentId);

    m_entityChildren[c->parentId].insert(c->entityId());
    addChildToComponent(sg, *parent, std::move(c));
  }

  m_components.insert(std::make_pair(ptr->entityId(), ptr));
}

//===========================================
// Scene::isRoot
//===========================================
bool Scene::isRoot(const CRenderSpatial& c) const {
  if (c.kind != CRenderSpatialKind::REGION) {
    return false;
  }
  if (sg.rootRegion == nullptr) {
    return false;
  }
  const Region* ptr = dynamic_cast<const Region*>(&c);
  return ptr == sg.rootRegion.get();
}

//===========================================
// Scene::removeEntity_r
//===========================================
void Scene::removeEntity_r(entityId_t id) {
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
// Scene::removeEntity
//===========================================
void Scene::removeEntity(entityId_t id) {
  auto it = m_components.find(id);
  if (it == m_components.end()) {
    return;
  }

  CRenderSpatial& c = *it->second;
  auto jt = m_components.find(c.parentId);

  if (jt != m_components.end()) {
    CRenderSpatial& parent = *jt->second;
    removeChildFromComponent(sg, parent, c);
  }
  else {
    assert(isRoot(c));
  }

  removeEntity_r(id);
}
