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


ostream& operator<<(ostream& os, CSpatialKind kind) {
  switch (kind) {
    case CSpatialKind::ZONE: os << "ZONE"; break;
    case CSpatialKind::WALL: os << "WALL"; break;
    case CSpatialKind::JOINING_EDGE: os << "JOINING_EDGE"; break;
    case CSpatialKind::FLOOR_DECAL: os << "FLOOR_DECAL"; break;
    case CSpatialKind::WALL_DECAL: os << "WALL_DECAL"; break;
    case CSpatialKind::SPRITE: os << "SPRITE"; break;
  }
  return os;
}


//===========================================
// forEachConstZone
//===========================================
void forEachConstZone(const CZone& zone, function<void(const CZone&)> fn) {
  fn(zone);
  std::for_each(zone.children.begin(), zone.children.end(),
    [&](const unique_ptr<CZone>& r) {

    forEachConstZone(*r, fn);
  });
}

//===========================================
// forEachZone
//===========================================
void forEachZone(CZone& zone, function<void(CZone&)> fn) {
  fn(zone);
  std::for_each(zone.children.begin(), zone.children.end(),
    [&](unique_ptr<CZone>& r) {

    forEachZone(*r, fn);
  });
}

//===========================================
// getNextZone
//===========================================
static CZone* getNextZone(const CZone& current, const JoiningEdge& je) {
  return je.zoneA == &current ? je.zoneB : je.zoneA;
}

//===========================================
// canStepAcross
//===========================================
static bool canStepAcross(const Player& player, const CZone& currentZone,
  const JoiningEdge& je) {

  CZone* nextZone = getNextZone(currentZone, je);

  bool canStep = nextZone->floorHeight - player.feetHeight() <= PLAYER_STEP_HEIGHT;
  bool hasHeadroom = player.headHeight() < nextZone->ceilingHeight;

  return canStep && hasHeadroom;
}

//===========================================
// intersectWall
//===========================================
static bool intersectWall(const CZone& zone, const Circle& circle, const Player& player) {
  bool b = false;

  forEachConstZone(zone, [&](const CZone& r) {
    if (!b) {
      for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
        const Edge& edge = **it;

        if (edge.kind == CSpatialKind::JOINING_EDGE) {
          const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

          //assert(&zone == je.zoneA || &zone == je.zoneB);
          if (&zone != je.zoneA && &zone != je.zoneB) {
            continue;
          }

          if (canStepAcross(player, zone, je)) {
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
static Vec2f getDelta(const CZone& zone, const Point& camPos, const Player& player,
  double radius, const Vec2f& dv) {

  Circle circle{camPos + dv, radius};
  LineSegment ray(camPos, camPos + dv);

  Vec2f dv_ = dv;

  assert(zone.parent != nullptr);

  bool abortLoop = false;
  forEachConstZone(*zone.parent, [&](const CZone& r) {
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

          if (edge.kind == CSpatialKind::JOINING_EDGE) {
            const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

            if (&zone != je.zoneA && &zone != je.zoneB) {
              continue;
            }

            if (canStepAcross(player, zone, je)) {
              continue;
            }
          }

          // Erase component in direction of wall
          Matrix m(-atan(edge.lseg.line().m), Vec2f());
          dv_ = m * dv;
          dv_.y = 0;
          dv_ = m.inverse() * dv_;

          if (intersectWall(zone, Circle{camPos + dv_, radius}, player)) {
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

  CZone& currentZone = getCurrentZone();

  dv = getDelta(currentZone, cam.pos, *sg.player, radius, dv);
  Circle circle{cam.pos + dv, radius};

  assert(currentZone.parent != nullptr);

  bool abortLoop = false;
  forEachConstZone(*currentZone.parent, [&](const CZone& zone) {
    if (abortLoop) {
      return;
    }

    int nIntersections = 0;
    double nearestX = 999999.9;
    CZone* nextZone = nullptr;
    Point p;

    for (auto it = zone.edges.begin(); it != zone.edges.end(); ++it) {
      const Edge& edge = **it;

      if (lineSegmentCircleIntersect(circle, edge.lseg)) {
        assert(edge.kind == CSpatialKind::JOINING_EDGE);
        const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

        LineSegment ray(cam.pos, cam.pos + dv);
        Point p_;
        bool crossesLine = lineSegmentIntersect(ray, edge.lseg, p_);

        if (crossesLine) {
          ++nIntersections;

          double dist = distance(cam.pos, p_);
          if (dist < nearestX) {
            nextZone = getNextZone(currentZone, je);
            p = p_;

            nearestX = dist;
          }
        }
      }
    }

    if (nIntersections > 0) {
      sg.player->currentRegion = nextZone->entityId();
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
  if (!sg.player->aboveGround(getCurrentZone())) {
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
  CZone& currentZone = getCurrentZone();

  if (fabs(sg.player->vVelocity) > 0.001 || sg.player->aboveGround(currentZone)) {
    double a = -600.0;
    double dt = 1.0 / m_frameRate;
    double dv = dt * a;
    sg.player->vVelocity += dv;
    double dy = sg.player->vVelocity * dt;

    sg.player->changeHeight(currentZone, dy);

    if (!sg.player->aboveGround(currentZone)) {
      sg.player->vVelocity = 0;
    }
  }
}

//===========================================
// Scene::buoyancy
//===========================================
void Scene::buoyancy() {
  CZone& currentZone = getCurrentZone();

  if (sg.player->feetHeight() + 0.1 < currentZone.floorHeight) {
    double dy = 150.0 / m_frameRate;
    sg.player->changeHeight(currentZone, dy);
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
static void getEntitiesInRadius_r(const CZone& zone, const Circle& circle,
  set<entityId_t>& entities) {

  for (auto it = zone.edges.begin(); it != zone.edges.end(); ++it) {
    if (overlapsCircle(circle, **it)) {
      entities.insert((*it)->entityId());
      entities.insert(zone.entityId());
    }
  }

  for (auto it = zone.sprites.begin(); it != zone.sprites.end(); ++it) {
    if (overlapsCircle(circle, **it)) {
      entities.insert((*it)->entityId());
    }
  }

  for (auto it = zone.floorDecals.begin(); it != zone.floorDecals.end(); ++it) {
    if (overlapsCircle(circle, **it)) {
      entities.insert((*it)->entityId());
    }
  }

  for (auto it = zone.children.begin(); it != zone.children.end(); ++it) {
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
// connectSubzones_r
//===========================================
static void connectSubzones_r(CZone& zone) {
  if (zone.children.size() == 0) {
    return;
  }

  for (auto it = zone.children.begin(); it != zone.children.end(); ++it) {
    CZone& r = **it;
    connectSubzones_r(r);

    for (auto jt = r.edges.begin(); jt != r.edges.end(); ++jt) {
      if ((*jt)->kind == CSpatialKind::JOINING_EDGE) {
        JoiningEdge* je = dynamic_cast<JoiningEdge*>(*jt);
        assert(je != nullptr);

        bool hasTwin = false;
        forEachZone(zone, [&](CZone& r_) {
          if (!hasTwin) {
            if (&r_ != &r) {
              for (auto lt = r_.edges.begin(); lt != r_.edges.end(); ++lt) {
                if ((*lt)->kind == CSpatialKind::JOINING_EDGE) {
                  JoiningEdge* other = dynamic_cast<JoiningEdge*>(*lt);
                  assert(other != nullptr);

                  if (je == other) {
                    hasTwin = true;
                    break;
                  }

                  if (areTwins(*je, *other)) {
                    hasTwin = true;

                    je->joinId = other->joinId;
                    je->zoneA = other->zoneA = &r;
                    je->zoneB = other->zoneB = &r_;

                    break;
                  }
                }
              }
            }
          }
        });

        if (!hasTwin) {
          je->zoneA = &r;
          je->zoneB = &zone;
        }
      }
    }
  };
}

//===========================================
// Scene::connectZones
//===========================================
void Scene::connectZones() {
  connectSubzones_r(*sg.rootZone);
}

//===========================================
// Scene::getEntitiesInRadius
//===========================================
set<entityId_t> Scene::getEntitiesInRadius(double radius) const {
  set<entityId_t> entities;

  const Point& pos = sg.player->pos();
  Circle circle{pos, radius};

  forEachConstZone(*getCurrentZone().parent, [&](const CZone& zone) {
    getEntitiesInRadius_r(zone, circle, entities);
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
// addToZone
//===========================================
static void addToZone(SceneGraph& sg, CZone& zone, pCSpatial_t child) {
  switch (child->kind) {
    case CSpatialKind::ZONE: {
      pCZone_t ptr(dynamic_cast<CZone*>(child.release()));
      ptr->parent = &zone;
      zone.children.push_back(std::move(ptr));
      break;
    }
    case CSpatialKind::JOINING_EDGE:
    case CSpatialKind::WALL: {
      pEdge_t ptr(dynamic_cast<Edge*>(child.release()));
      zone.edges.push_back(ptr.get());
      sg.edges.push_back(std::move(ptr));
      break;
    }
    case CSpatialKind::FLOOR_DECAL: {
      pFloorDecal_t ptr(dynamic_cast<FloorDecal*>(child.release()));
      zone.floorDecals.push_back(std::move(ptr));
      break;
    }
    case CSpatialKind::SPRITE: {
      pSprite_t ptr(dynamic_cast<Sprite*>(child.release()));
      zone.sprites.push_back(std::move(ptr));
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to zone");
  }
}

//===========================================
// addToWall
//===========================================
static void addToWall(Wall& edge, pCSpatial_t child) {
  switch (child->kind) {
    case CSpatialKind::WALL_DECAL: {
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
static void addChildToComponent(SceneGraph& sg, CSpatial& parent, pCSpatial_t child) {
  switch (parent.kind) {
    case CSpatialKind::ZONE:
      addToZone(sg, dynamic_cast<CZone&>(parent), std::move(child));
      break;
    case CSpatialKind::WALL:
      addToWall(dynamic_cast<Wall&>(parent), std::move(child));
      break;
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to component of kind "
        << parent.kind);
  };
}

//===========================================
// removeFromZone
//===========================================
static void removeFromZone(SceneGraph& sg, CZone& zone, const CSpatial& child) {
  switch (child.kind) {
    case CSpatialKind::ZONE: {
      zone.children.remove_if([&](const pCZone_t& e) {
        return e.get() == dynamic_cast<const CZone*>(&child);
      });
      break;
    }
    case CSpatialKind::JOINING_EDGE:
    case CSpatialKind::WALL: {
      zone.edges.remove_if([&](const Edge* e) {
        return e == dynamic_cast<const Edge*>(&child);
      });
      sg.edges.remove_if([&](const pEdge_t& e) {
        return e.get() == dynamic_cast<const Edge*>(&child);
      });
      break;
    }
    case CSpatialKind::FLOOR_DECAL: {
      zone.floorDecals.remove_if([&](const pFloorDecal_t& e) {
        return e.get() == dynamic_cast<const FloorDecal*>(&child);
      });
      break;
    }
    case CSpatialKind::SPRITE: {
      zone.sprites.remove_if([&](const pSprite_t& e) {
        return e.get() == dynamic_cast<const Sprite*>(&child);
      });
      break;
    }
    default:
      EXCEPTION("Cannot add component of kind " << child.kind << " to zone");
  }
}

//===========================================
// removeFromWall
//===========================================
static void removeFromWall(Wall& edge, const CSpatial& child) {
  switch (child.kind) {
    case CSpatialKind::WALL_DECAL: {
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
static void removeChildFromComponent(SceneGraph& sg, CSpatial& parent,
  const CSpatial& child) {

  switch (parent.kind) {
    case CSpatialKind::ZONE:
      removeFromZone(sg, dynamic_cast<CZone&>(parent), child);
      break;
    case CSpatialKind::WALL:
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
  if (component->kind() != ComponentKind::C_SPATIAL) {
    EXCEPTION("Component is not of kind C_SPATIAL");
  }

  CSpatial* ptr = dynamic_cast<CSpatial*>(component.release());
  pCSpatial_t c(ptr);

  if (c->parentId == -1) {
    if (sg.rootZone) {
      EXCEPTION("Root zone already set");
    }

    if (c->kind != CSpatialKind::ZONE) {
      EXCEPTION("Component has no parent; Only zones can be root");
    }

    pCZone_t z(dynamic_cast<CZone*>(c.release()));

    sg.rootZone = std::move(z);
    m_components.clear();
  }
  else {
    auto it = m_components.find(c->parentId);
    if (it == m_components.end()) {
      EXCEPTION("Could not find parent component with id " << c->parentId);
    }

    CSpatial* parent = it->second;
    assert(parent->entityId() == c->parentId);

    m_entityChildren[c->parentId].insert(c->entityId());
    addChildToComponent(sg, *parent, std::move(c));
  }

  m_components.insert(std::make_pair(ptr->entityId(), ptr));
}

//===========================================
// Scene::isRoot
//===========================================
bool Scene::isRoot(const CSpatial& c) const {
  if (c.kind != CSpatialKind::ZONE) {
    return false;
  }
  if (sg.rootZone == nullptr) {
    return false;
  }
  const CZone* ptr = dynamic_cast<const CZone*>(&c);
  return ptr == sg.rootZone.get();
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

  CSpatial& c = *it->second;
  auto jt = m_components.find(c.parentId);

  if (jt != m_components.end()) {
    CSpatial& parent = *jt->second;
    removeChildFromComponent(sg, parent, c);
  }
  else {
    assert(isRoot(c));
  }

  removeEntity_r(id);
}
