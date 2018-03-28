#include <string>
#include <cassert>
#include <sstream>
#include <ostream>
#include <list>
#include <iterator>
#include <deque>
#include "raycast/spatial_system.hpp"
#include "raycast/geometry.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/tween_curves.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/time_service.hpp"
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
using std::pair;
using std::make_pair;
using std::find_if;
using std::for_each;
using std::vector;


static const double GRID_CELL_SIZE = 25.0;
static const double SNAP_DISTANCE = 4.0;
static const int MAX_ANCESTOR_SEARCH = 2;


ostream& operator<<(ostream& os, CSpatialKind kind) {
  switch (kind) {
    case CSpatialKind::ZONE: os << "ZONE"; break;
    case CSpatialKind::HARD_EDGE: os << "HARD_EDGE"; break;
    case CSpatialKind::SOFT_EDGE: os << "SOFT_EDGE"; break;
    case CSpatialKind::H_RECT: os << "H_RECT"; break;
    case CSpatialKind::V_RECT: os << "V_RECT"; break;
    case CSpatialKind::PATH: os << "PATH"; break;
  }
  return os;
}


//===========================================
// forEachConstZone
//===========================================
void forEachConstZone(const CZone& zone, function<void(const CZone&)> fn) {
  fn(zone);
  for_each(zone.children.begin(), zone.children.end(), [&](const unique_ptr<CZone>& r) {
    forEachConstZone(*r, fn);
  });
}

//===========================================
// forEachZone
//===========================================
void forEachZone(CZone& zone, function<void(CZone&)> fn) {
  fn(zone);
  for_each(zone.children.begin(), zone.children.end(), [&](unique_ptr<CZone>& r) {
    forEachZone(*r, fn);
  });
}

//===========================================
// nthAncestor
//===========================================
static CZone& nthAncestor(CZone& z, int n) {
  return (n <= 0 || z.parent == nullptr) ? z : nthAncestor(*z.parent, n - 1);
}

//===========================================
// nthConstAncestor
//===========================================
static const CZone& nthConstAncestor(const CZone& z, int n) {
  return (n <= 0 || z.parent == nullptr) ? z : nthConstAncestor(*z.parent, n - 1);
}

//===========================================
// getNextZone
//===========================================
static CZone* getNextZone(const CZone& current, const CSoftEdge& se) {
  return se.zoneA == &current ? se.zoneB : se.zoneA;
}

//===========================================
// canStepAcrossEdge
//===========================================
static bool canStepAcrossEdge(const CZone& zone, double height, const Size& bodySize,
  const CEdge& edge) {

  if (edge.kind != CSpatialKind::SOFT_EDGE) {
    return false;
  }

  const CSoftEdge& se = dynamic_cast<const CSoftEdge&>(edge);
  CZone* nextZone = getNextZone(zone, se);

  bool canStep = nextZone->floorHeight - height <= PLAYER_STEP_HEIGHT;
  bool hasHeadroom = !nextZone->hasCeiling || (height + bodySize.y < nextZone->ceilingHeight);

  return canStep && hasHeadroom;
}

//===========================================
// pathBlocked
//
// Returns true if a body moving through vector v is blocked by an edge it cannot cross.
//===========================================
static bool pathBlocked(const CZone& zone, const Point& pos, double height, const Size& bodySize,
  const Vec2f v) {

  LineSegment lseg(pos, pos + v);

  bool b = false;

  forEachConstZone(nthConstAncestor(zone, MAX_ANCESTOR_SEARCH), [&](const CZone& r) {
    for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
      const CEdge& edge = **it;

      Point p;
      if (lineSegmentIntersect(lseg, edge.lseg, p)) {
        if (!canStepAcrossEdge(zone, height, bodySize, edge)) {
          b = true;
          return;
        }
      }
    }
  });

  return b;
}

//===========================================
// getDelta
//
// Takes the vector the player wants to move in (oldV) and returns a modified vector that doesn't
// allow the player within radius units of a wall.
//===========================================
static Vec2f getDelta(const CVRect& body, double height, double radius, const Vec2f& oldV,
  int depth = 0) {

  if (depth > 4) {
    return Vec2f(0, 0);
  }

  const Point& pos = body.pos;
  const CZone& zone = *body.zone;

  assert(zone.parent != nullptr);

  bool collision = false;
  Vec2f newV(9999, 9999); // Value closest to oldV
  Circle circle{pos + oldV, radius};

  forEachConstZone(nthConstAncestor(zone, MAX_ANCESTOR_SEARCH), [&](const CZone& r) {
    for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
      const CEdge& edge = **it;

      // If moving by oldV will intersect something, we need to change it
      if (lineSegmentCircleIntersect(circle, edge.lseg) &&
        !canStepAcrossEdge(*body.zone, height, body.size, edge)) {

        collision = true;

        // The body's position if it were moved by oldV
        Point nextPos = pos + oldV;

        Point X = projectionOntoLine(edge.lseg.line(), nextPos);
        Vec2f toEdge = nextPos - X;

        Vec2f adjustment = normalise(toEdge) * (radius - length(toEdge)) * 1.0001;

        Vec2f v = getDelta(body, height, radius, oldV + adjustment, depth + 1);

        // This should prevent corner cutting
        if (!pathBlocked(zone, pos, height, body.size, v)
          && length(v - oldV) < length(newV - oldV)) {

          newV = v;
        }
      }
    }
  });

  return collision ? newV : oldV;
}

//===========================================
// playerBounce
//===========================================
static void playerBounce(SpatialSystem& spatialSystem, TimeService& timeService, double frameRate) {
  double y = 5.0;
  double duration = 0.33;
  int nFrames = duration * timeService.frameRate;
  double dy = y / (0.5 * nFrames);

  timeService.addTween(Tween{[&, nFrames, dy](long i, double, double) -> bool {
    if (i < 0.5 * nFrames) {
      spatialSystem.sg.player->changeTallness(dy);
    }
    else {
      spatialSystem.sg.player->changeTallness(-dy);
    }
    return i < nFrames;
  }, [](long, double, double) {}}, "player_bounce");
}

//===========================================
// overlapsCircle
//===========================================
static bool overlapsCircle(const Circle& circle, const CEdge& edge) {
  return lineSegmentCircleIntersect(circle, edge.lseg); // TODO
}

//===========================================
// overlapsCircle
//===========================================
static bool overlapsCircle(const Circle& circle, const CVRect& vRect) {
  return distance(circle.pos, vRect.pos) <= circle.radius;
}

//===========================================
// overlapsCircle
//===========================================
static bool overlapsCircle(const Circle& circle, const LineSegment& wall, const CVRect& vRect) {
  Vec2f v = normalise(wall.B - wall.A);
  return distance(circle.pos, wall.A + v * vRect.pos.x) <= circle.radius;
}

//===========================================
// overlapsCircle
//===========================================
static bool overlapsCircle(const Circle& circle, const CHRect& hRect) {
  // TODO

  return false;
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
    case CSpatialKind::SOFT_EDGE:
    case CSpatialKind::HARD_EDGE: {
      pCEdge_t ptr(dynamic_cast<CEdge*>(child.release()));
      zone.edges.push_back(ptr.get());
      sg.edges.push_back(std::move(ptr));
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
    case CSpatialKind::PATH: {
      pCPath_t ptr(dynamic_cast<CPath*>(child.release()));
      zone.paths.push_back(std::move(ptr));
      break;
    }
    default: {
      EXCEPTION("Cannot add component of kind " << child->kind << " to zone");
    }
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
    default: {
      EXCEPTION("Cannot add component of kind " << child->kind << " to HardEdge");
    }
  }
}

//===========================================
// addToSoftEdge
//===========================================
static void addToSoftEdge(CSoftEdge& edge, pCSpatial_t child) {
  switch (child->kind) {
    case CSpatialKind::V_RECT: {
      pCVRect_t ptr(dynamic_cast<CVRect*>(child.release()));
      edge.vRects.push_back(std::move(ptr));
      break;
    }
    default: {
      EXCEPTION("Cannot add component of kind " << child->kind << " to SoftEdge");
    }
  }
}

//===========================================
// SpatialSystem::addChildToComponent
//===========================================
void SpatialSystem::addChildToComponent(CSpatial& parent, pCSpatial_t child) {
  const CSpatial* ptr = child.get();

  switch (parent.kind) {
    case CSpatialKind::ZONE:
      addToZone(sg, dynamic_cast<CZone&>(parent), std::move(child));
      break;
    case CSpatialKind::HARD_EDGE:
      addToHardEdge(dynamic_cast<CHardEdge&>(parent), std::move(child));
      break;
    case CSpatialKind::SOFT_EDGE:
      addToSoftEdge(dynamic_cast<CSoftEdge&>(parent), std::move(child));
      break;
    default:
      EXCEPTION("Cannot add component of kind " << child->kind << " to component of kind "
        << parent.kind);
  };

  m_entityChildren[parent.entityId()].insert(ptr->entityId());
}

//===========================================
// removeFromZone
//===========================================
static bool removeFromZone(SceneGraph& sg, CZone& zone, const CSpatial& child, bool keepAlive) {
  bool found = false;

  switch (child.kind) {
    case CSpatialKind::ZONE: {
      auto it = find_if(zone.children.begin(), zone.children.end(), [&](const pCZone_t& e) {
        return e.get() == dynamic_cast<const CZone*>(&child);
      });
      if (it != zone.children.end()) {
        if (keepAlive) {
          it->release();
        }
        erase(zone.children, it);
        found = true;
      }
      break;
    }
    case CSpatialKind::SOFT_EDGE:
    case CSpatialKind::HARD_EDGE: {
      auto it = find_if(zone.edges.begin(), zone.edges.end(), [&](const CEdge* e) {
        return e == dynamic_cast<const CEdge*>(&child);
      });
      erase(zone.edges, it);
      auto jt = find_if(sg.edges.begin(), sg.edges.end(), [&](const pCEdge_t& e) {
        return e.get() == dynamic_cast<const CEdge*>(&child);
      });
      if (jt != sg.edges.end()) {
        if (keepAlive) {
          jt->release();
        }
        erase(sg.edges, jt);
        found = true;
      }
      break;
    }
    case CSpatialKind::H_RECT: {
      auto it = find_if(zone.hRects.begin(), zone.hRects.end(), [&](const pCHRect_t& e) {
        return e.get() == dynamic_cast<const CHRect*>(&child);
      });
      if (it != zone.hRects.end()) {
        if (keepAlive) {
          it->release();
        }
        erase(zone.hRects, it);
        found = true;
      }
      break;
    }
    case CSpatialKind::V_RECT: {
      auto it = find_if(zone.vRects.begin(), zone.vRects.end(), [&](const pCVRect_t& e) {
        return e.get() == dynamic_cast<const CVRect*>(&child);
      });
      if (it != zone.vRects.end()) {
        if (keepAlive) {
          it->release();
        }
        erase(zone.vRects, it);
        found = true;
      }
      break;
    }
    case CSpatialKind::PATH: {
      auto it = find_if(zone.paths.begin(), zone.paths.end(), [&](const pCPath_t& e) {
        return e.get() == dynamic_cast<const CPath*>(&child);
      });
      if (it != zone.paths.end()) {
        if (keepAlive) {
          it->release();
        }
        erase(zone.paths, it);
        found = true;
      }
      break;
    }
    default: {
      EXCEPTION("Cannot add component of kind " << child.kind << " to zone");
    }
  }

  return found;
}

//===========================================
// removeFromHardEdge
//===========================================
static bool removeFromHardEdge(CHardEdge& edge, const CSpatial& child, bool keepAlive) {
  bool found = false;

  switch (child.kind) {
    case CSpatialKind::V_RECT: {
      auto it = find_if(edge.vRects.begin(), edge.vRects.end(), [&](const pCVRect_t& e) {
        return e.get() == dynamic_cast<const CVRect*>(&child);
      });
      if (it != edge.vRects.end()) {
        if (keepAlive) {
          it->release();
        }
        erase(edge.vRects, it);
        found = true;
      }
      break;
    }
    default: {
      EXCEPTION("Cannot remove component of kind " << child.kind << " from HardEdge");
    }
  }

  return found;
}

//===========================================
// similar
//===========================================
static bool similar(const LineSegment& l1, const LineSegment& l2) {
  return (distance(l1.A, l2.A) <= SNAP_DISTANCE && distance(l1.B, l2.B) <= SNAP_DISTANCE)
    || (distance(l1.A, l2.B) <= SNAP_DISTANCE && distance(l1.B, l2.A) <= SNAP_DISTANCE);
}

//===========================================
// SpatialSystem::zone
//===========================================
CZone& SpatialSystem::zone(entityId_t entityId) {
  return const_cast<CZone&>(constZone(entityId));
}

//===========================================
// SpatialSystem::constZone
//===========================================
const CZone& SpatialSystem::constZone(entityId_t entityId) const {
  while (entityId != -1) {
    CSpatial& c = *m_components.at(entityId);

    if (c.kind == CSpatialKind::ZONE) {
      return dynamic_cast<CZone&>(c);
    }

    entityId = c.parentId;
  }

  EXCEPTION("Entity with id " << entityId << " is not inside a zone");
}

//===========================================
// SpatialSystem::isAncestor
//===========================================
bool SpatialSystem::isAncestor(entityId_t a, entityId_t b) const {
  if (a == -1 || b == -1) {
    return false;
  }

  const CSpatial& bComp = *m_components.at(b);
  if (bComp.parentId == -1) {
    return false;
  }

  const CSpatial* anc = m_components.at(bComp.parentId);

  while (anc->parentId != -1) {
    if (anc->entityId() == a) {
      return true;
    }

    anc = m_components.at(anc->parentId);
  };

  return false;
}

//===========================================
// SpatialSystem::areTwins
//===========================================
bool SpatialSystem::areTwins(const CSoftEdge& se1, const CSoftEdge& se2) const {
  return similar(se1.lseg, se2.lseg) &&
    !(isAncestor(se1.parentId, se2.parentId) || isAncestor(se2.parentId, se1.parentId));
}

//===========================================
// SpatialSystem::connectSubzones
//===========================================
void SpatialSystem::connectSubzones(CZone& zone) {
  forEachZone(zone, [&](CZone& r) {
    for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
      if ((*it)->kind == CSpatialKind::SOFT_EDGE) {
        CSoftEdge* se = dynamic_cast<CSoftEdge*>(*it);
        assert(se != nullptr);

        bool hasTwin = false;
        forEachZone(zone, [&](CZone& r_) {
          if (!hasTwin) {
            if (&r_ != &r) {
              for (auto lt = r_.edges.begin(); lt != r_.edges.end(); ++lt) {
                if ((*lt)->kind == CSpatialKind::SOFT_EDGE) {
                  CSoftEdge* other = dynamic_cast<CSoftEdge*>(*lt);

                  if (areTwins(*se, *other)) {
                    hasTwin = true;

                    se->joinId = other->joinId;
                    se->zoneA = &r;
                    se->zoneB = &r_;
                    se->lseg = other->lseg;
                    se->twinId = other->entityId();

                    break;
                  }

                  // If they're already joined by id (i.e. portals)
                  if (se->joinId != -1 && se->joinId == other->joinId) {
                    hasTwin = true;

                    se->zoneA = &r;
                    se->zoneB = &r_;
                    se->twinId = other->entityId();

                    double a1 = se->lseg.angle();
                    double a2 = other->lseg.angle();
                    double a = a2 - a1;

                    Matrix toOrigin(0, -se->lseg.A);
                    Matrix rotate(a, Vec2f(0, 0));
                    Matrix translate(0, other->lseg.A);
                    Matrix m = translate * rotate * toOrigin;

                    other->lseg.A = m * se->lseg.A;
                    other->lseg.B = m * se->lseg.B;

                    se->toTwin = m;

                    break;
                  }
                }
              }
            }
          }
        });

        if (!hasTwin) {
          assert(r.parent != nullptr);

          se->zoneA = &r;
          se->zoneB = r.parent;
        }
      }
    }
  });
}

//===========================================
// SpatialSystem::SpatialSystem
//===========================================
SpatialSystem::SpatialSystem(EntityManager& entityManager, TimeService& timeService,
  double frameRate)
  : m_entityManager(entityManager),
    m_timeService(timeService) {

  m_frameRate = frameRate;
}

//===========================================
// SpatialSystem::handleEvent
//===========================================
void SpatialSystem::handleEvent(const GameEvent& event) {
  if (event.name == "player_activate") {
    EPlayerActivateEntity e(*sg.player);
    const CZone& zone = getCurrentZone();
    const CVRect& body = dynamic_cast<const CVRect&>(getComponent(sg.player->body));

    double y = sg.player->feetHeight() - zone.floorHeight + 0.5 * body.size.y;

    m_entityManager.fireEvent(e, entitiesInRadius(zone, sg.player->pos(),
      sg.player->activationRadius, y));
  }
}

//===========================================
// SpatialSystem::vRotateCamera
//===========================================
void SpatialSystem::vRotateCamera(double da) {
  sg.player->vRotate(da);
}

//===========================================
// SpatialSystem::hRotateCamera
//===========================================
void SpatialSystem::hRotateCamera(double da) {
  sg.player->hRotate(da);
}

//===========================================
// SpatialSystem::crossZones
//
// Doesn't set the zone on the entity
//===========================================
void SpatialSystem::crossZones(entityId_t entityId, entityId_t oldZoneId, entityId_t newZoneId) {
  DBG_PRINT("Crossing from zone " << oldZoneId << " to zone " << newZoneId << "\n");

  if (oldZoneId == newZoneId) {
    return;
  }

  auto it = m_components.find(entityId);
  if (it != m_components.end()) {
    CSpatial& c = *it->second;
    CZone& oldZone = dynamic_cast<CZone&>(getComponent(oldZoneId));
    CZone& newZone = dynamic_cast<CZone&>(getComponent(newZoneId));

    if (removeChildFromComponent(oldZone, c, true)) {
      addChildToComponent(newZone, pCSpatial_t(&c));

      m_entityManager.broadcastEvent(EChangedZone(entityId, oldZoneId, newZoneId));
    }
  }
}

//===========================================
// SpatialSystem::relocateEntity
//===========================================
void SpatialSystem::relocateEntity(entityId_t id, CZone& zone, const Point& point) {
  auto it = m_components.find(id);
  if (it != m_components.end()) {
    CSpatial& c = *it->second;

    // Currently, only VRects can be moved
    if (c.kind == CSpatialKind::V_RECT) {
      CVRect& body = dynamic_cast<CVRect&>(c);

      crossZones(id, body.zone->entityId(), zone.entityId());

      body.zone = &zone;
      body.pos = point;
    }
  }
}

//===========================================
// SpatialSystem::moveEntity
//===========================================
void SpatialSystem::moveEntity(entityId_t id, Vec2f dv, double heightAboveFloor) {
  auto it = m_components.find(id);
  if (it != m_components.end()) {
    CSpatial& c = *it->second;

    // Currently, only VRects can be moved
    if (c.kind == CSpatialKind::V_RECT) {
      CVRect& body = dynamic_cast<CVRect&>(c);

      // TODO
      double radius = 10.0; //body.size.x * 0.5;

      dv = getDelta(body, body.zone->floorHeight + heightAboveFloor, radius, dv);

      assert(body.zone->parent != nullptr);

      list<const CSoftEdge*> edgesCrossed;
      LineSegment lseg(body.pos, body.pos + dv);

      bool abort = false;
      forEachConstZone(nthAncestor(*body.zone, MAX_ANCESTOR_SEARCH), [&](const CZone& r) {
        if (abort) {
          return;
        }

        for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
          const CEdge& edge = **it;

          Point X;
          if (lineSegmentIntersect(lseg, edge.lseg, X)) {
            if (edge.kind != CSpatialKind::SOFT_EDGE) {
              // TODO: Find out how this is possible and prevent it

              DBG_PRINT("Warning: Crossed edge is not soft edge\n");
              abort = true;
              return;
            }

            const CSoftEdge& se = dynamic_cast<const CSoftEdge&>(edge);
            edgesCrossed.push_back(&se);
          }
        }
      });

      if (abort) {
        return;
      }

      Matrix m;
      map<entityId_t, bool> crossed;
      list<const CSoftEdge*> excluded;
      for (auto jt = edgesCrossed.begin(); jt != edgesCrossed.end(); ++jt) {
        const CSoftEdge& se = dynamic_cast<const CSoftEdge&>(**jt);

        if (se.zoneA->entityId() != body.zone->entityId() &&
          se.zoneB->entityId() != body.zone->entityId()) {

          excluded.push_back(&se);
          continue;
        }

        if (!crossed[se.joinId]) {
          CZone* nextZone = getNextZone(*body.zone, se);

          crossZones(id, body.zone->entityId(), nextZone->entityId());
          body.zone = nextZone;

          m = m * se.toTwin;

          if (se.joinId != -1) {
            crossed[se.joinId] = true;
          }
        }

        edgesCrossed.insert(edgesCrossed.end(), excluded.begin(), excluded.end());
        excluded.clear();
      }

      body.pos = body.pos + dv;
      body.pos = m * body.pos;
      body.angle += m.a();
    }
  }
}

//===========================================
// SpatialSystem::movePlayer
//===========================================
void SpatialSystem::movePlayer(const Vec2f& v) {
  Player& player = *sg.player;
  const CVRect& body = dynamic_cast<const CVRect&>(getComponent(player.body));
  const CZone& zone =  getCurrentZone();

  Vec2f dv(cos(body.angle) * v.x - sin(body.angle) * v.y,
    sin(body.angle) * v.x + cos(body.angle) * v.y);

  moveEntity(player.body, dv, player.feetHeight() - zone.floorHeight);

  playerBounce(*this, m_timeService, m_frameRate);

  Point pos = player.pos();
  Vec2i cell(pos.x / GRID_CELL_SIZE, pos.y / GRID_CELL_SIZE);

  if (cell != m_playerCell) {
    m_playerCell = cell;

    EPlayerMove e(player);
    m_entityManager.fireEvent(e, entitiesInRadius(zone, player.pos(), player.collectionRadius,
      player.feetHeight() - zone.floorHeight));
  }
}

//===========================================
// SpatialSystem::shortestPath
//===========================================
vector<Point> SpatialSystem::shortestPath(entityId_t entityA, entityId_t entityB,
  double radius) const {

  const CSpatial& compA = dynamic_cast<const CSpatial&>(getComponent(entityA));
  if (compA.kind != CSpatialKind::V_RECT) {
    EXCEPTION("Component not of type V_RECT");
  }

  const CSpatial& compB = dynamic_cast<const CSpatial&>(getComponent(entityB));
  if (compB.kind != CSpatialKind::V_RECT) {
    EXCEPTION("Component not of type V_RECT");
  }

  const CVRect& rectA = dynamic_cast<const CVRect&>(compA);
  const CVRect& rectB = dynamic_cast<const CVRect&>(compB);

  return shortestPath(rectA.pos, rectB.pos, radius);
}

//===========================================
// SpatialSystem::shortestPath
//===========================================
vector<Point> SpatialSystem::shortestPath(const Point& A, const Point& B, double radius) const {
  // TODO: A*

  vector<Point> path;
  path.push_back(B);

  return path;
}

//===========================================
// SpatialSystem::jump
//===========================================
void SpatialSystem::jump() {
  if (!sg.player->aboveGround(getCurrentZone())) {
    sg.player->vVelocity = 220;
  }
}

//===========================================
// SpatialSystem::gravity
//===========================================
void SpatialSystem::gravity() {
  CZone& currentZone = getCurrentZone();
  Player& player = *sg.player;

  double diff = (player.feetHeight() - 0.1) - currentZone.floorHeight;

  if (fabs(player.vVelocity) > 0.001 || diff > 0.0) {
    double a = -600.0;
    double dt = 1.0 / m_frameRate;
    double dv = dt * a;
    player.vVelocity += dv;
    double dy = player.vVelocity * dt;

    double dy_ = dy > 0.0 ? dy : smallest<double>(diff, dy);

    player.changeHeight(currentZone, dy_);
    moveEntity(player.body, Vec2f(0, 0), player.feetHeight() - currentZone.floorHeight);

    if (!player.aboveGround(currentZone)) {
      player.vVelocity = 0;
    }
  }
}

//===========================================
// SpatialSystem::buoyancy
//===========================================
void SpatialSystem::buoyancy() {
  CZone& currentZone = getCurrentZone();

  double diff = currentZone.floorHeight - (sg.player->feetHeight() + 0.1);

  if (diff > 0.0) {
    double dy = smallest<double>(diff, 150.0 / m_frameRate);
    sg.player->changeHeight(currentZone, dy);
  }
}

//===========================================
// SpatialSystem::connectZones
//===========================================
void SpatialSystem::connectZones() {
  connectSubzones(*sg.rootZone);
}

//===========================================
// SpatialSystem::findIntersections_r
//===========================================
void SpatialSystem::findIntersections_r(const Point& point, const Vec2f& dir, const Matrix& matrix,
  entityId_t parentId, list<pIntersection_t>& intersections, set<entityId_t>& visited,
  double cullNearerThan) const {

  const CZone& zone = constZone(parentId);
  const CSpatial& parent = *m_components.at(parentId);
  Matrix invMatrix = matrix.inverse();

  LineSegment ray(point, 10000.0 * dir);
  visited.insert(parentId);

  auto& children = m_entityChildren.at(parentId);
  for (entityId_t childId : children) {
    const CSpatial& c = *m_components.at(childId);

    switch (c.kind) {
      case CSpatialKind::ZONE: {
        if (visited.count(childId) == 0) {
          findIntersections_r(point, dir, matrix, childId, intersections, visited,
            cullNearerThan);
        }
        break;
      }
      case CSpatialKind::V_RECT: {
        const CVRect& vRect = dynamic_cast<const CVRect&>(c);

        if (parent.kind == CSpatialKind::ZONE) {
          Point pos = matrix * vRect.pos;
          double w = vRect.size.x;
          LineSegment lseg(Point(pos.x, pos.y - 0.5 * w), Point(pos.x, pos.y + 0.5 * w));

          Point pt;
          if (lineSegmentIntersect(ray, lseg, pt)) {
            if (pt.x < cullNearerThan) {
              continue;
            }

            Intersection* X = new Intersection(CSpatialKind::V_RECT, parent.kind);
            intersections.push_back(pIntersection_t(X));
            X->entityId = vRect.entityId();
            X->point_rel = pt;
            X->point_wld = invMatrix * pt;
            X->viewPoint = invMatrix * point;
            X->distanceFromOrigin = pt.x;
            X->distanceAlongTarget = distance(lseg.A, pt);
            X->zoneA = X->zoneB = zone.entityId();
            X->heightRanges = make_pair(Range(vRect.zone->floorHeight,
              vRect.zone->floorHeight + vRect.size.y), Range(0, 0));
          }
        }
        else if (parent.kind == CSpatialKind::HARD_EDGE || parent.kind == CSpatialKind::SOFT_EDGE) {
          const CEdge& edge = dynamic_cast<const CEdge&>(parent);
          LineSegment wallLseg = transform(edge.lseg, matrix);

          Vec2f v = normalise(wallLseg.B - wallLseg.A);

          Point pos = wallLseg.A + v * vRect.pos.x;
          double w = vRect.size.x;
          LineSegment lseg(pos, pos + w * v);

          Point pt;
          if (lineSegmentIntersect(ray, lseg, pt)) {
            if (pt.x < cullNearerThan) {
              continue;
            }

            assert(parent.parentId == zone.entityId());

            pt.x -= 0.01;

            double vRectFloorH = vRect.zone->floorHeight;
            if (edge.kind == CSpatialKind::SOFT_EDGE) {
              const CSoftEdge& se = dynamic_cast<const CSoftEdge&>(edge);
              vRectFloorH = smallest(se.zoneA->floorHeight, se.zoneB->floorHeight);
            }
            double y0 = vRectFloorH + vRect.pos.y;
            double y1 = y0 + vRect.size.y;

            Intersection* X = new Intersection(CSpatialKind::V_RECT, parent.kind);
            intersections.push_back(pIntersection_t(X));
            X->entityId = vRect.entityId();
            X->point_rel = pt;
            X->point_wld = invMatrix * pt;
            X->viewPoint = invMatrix * point;
            X->distanceFromOrigin = pt.x;
            X->distanceAlongTarget = distance(lseg.A, pt);
            X->zoneA = X->zoneB = zone.entityId();
            X->heightRanges = make_pair(Range(y0, y1), Range(0, 0));
          }
        }
        break;
      }
      case CSpatialKind::HARD_EDGE:
      case CSpatialKind::SOFT_EDGE: {
        const CEdge& edge = dynamic_cast<const CEdge&>(c);
        LineSegment lseg = transform(edge.lseg, matrix);

        Point pt;
        if (lineSegmentIntersect(ray, lseg, pt)) {
          if (pt.x < cullNearerThan) {
            continue;
          }

          if (visited.count(childId) == 0) {
            findIntersections_r(point, dir, matrix, childId, intersections, visited, cullNearerThan);
          }

          Intersection* X = new Intersection(edge.kind, parent.kind);
          X->entityId = edge.entityId();
          X->point_rel = pt;
          X->point_wld = invMatrix * pt;
          X->viewPoint = invMatrix * point;
          X->distanceFromOrigin = pt.x;
          X->distanceAlongTarget = distance(lseg.A, pt);
          X->zoneA = zone.entityId();

          if (edge.kind == CSpatialKind::HARD_EDGE) {
            X->zoneB = zone.parent != nullptr ? zone.parent->entityId() : X->zoneA;
            X->heightRanges = make_pair(Range(-10000, 10000), Range(0, 0)); // TODO
            intersections.push_back(pIntersection_t(X));
          }
          else if (edge.kind == CSpatialKind::SOFT_EDGE) {
            const CSoftEdge& se = dynamic_cast<const CSoftEdge&>(edge);
            const CZone& next = se.zoneA == &zone ? *se.zoneB : *se.zoneA;

            if (visited.count(se.joinId) > 0) {
              continue;
            }

            visited.insert(se.joinId);

            X->zoneB = next.entityId();
            X->heightRanges = make_pair(Range(se.zoneA->floorHeight, se.zoneB->floorHeight),
              Range(se.zoneA->ceilingHeight, se.zoneB->ceilingHeight));

            Matrix mat = matrix;
            double cullNearerThan_ = cullNearerThan;

            if (se.isPortal) {
              mat = (se.toTwin * invMatrix).inverse();
              cullNearerThan_ = X->distanceFromOrigin;
            }

            intersections.push_back(pIntersection_t(X));

            if (visited.count(next.entityId()) == 0) {
              findIntersections_r(point, dir, mat, next.entityId(), intersections, visited, cullNearerThan_);
            }
          }
          else {
            EXCEPTION("Unexpected intersection type");
          }
        }
        break;
      }
      default: break;
    }
  }
}

//===========================================
// SpatialSystem::entitiesAlongRay
//===========================================
list<pIntersection_t> SpatialSystem::entitiesAlongRay(const Vec2f& ray) const {
  const Camera& camera = sg.player->camera();

  Matrix matrix = camera.matrix().inverse();
  list<pIntersection_t> intersections = entitiesAlongRay(getCurrentZone(), Point(0, 0), ray,
    matrix);

  auto it = find_if(intersections.begin(), intersections.end(), [&](const pIntersection_t& i) {
    return i->entityId == sg.player->body;
  });

  erase(intersections, it);

  return intersections;
}

//===========================================
// SpatialSystem::entitiesAlongRay
//===========================================
list<pIntersection_t> SpatialSystem::entitiesAlongRay(const CZone& zone, const Point& pos,
  const Vec2f& dir, const Matrix& matrix) const {

  list<pIntersection_t> intersections;
  set<entityId_t> visited;
  findIntersections_r(pos, dir, matrix, zone.entityId(), intersections, visited);

  intersections.sort([](const pIntersection_t& a, const pIntersection_t& b) {
    return a->distanceFromOrigin < b->distanceFromOrigin;
  });

  std::list<pIntersection_t> Xs;
  std::list<pIntersection_t> excluded;
  entityId_t currentZone = zone.entityId();

  while (!intersections.empty()) {
    const auto& X = intersections.front();

    if (X->zoneA == currentZone || X->zoneB == currentZone) {
      auto other = currentZone == X->zoneA ? X->zoneB : X->zoneA;

      Xs.push_back(std::move(intersections.front()));
      intersections.pop_front();

      currentZone = other;

      for (auto ex = excluded.rbegin(); ex != excluded.rend(); ++ex) {
        intersections.push_front(std::move(*ex));
      }
      excluded.clear();
    }
    else {
      excluded.push_back(std::move(intersections.front()));
      intersections.pop_front();
    }
  }

  if (excluded.size() > 0) {
    // TODO: Find out why this happens
    //DBG_PRINT("Warning: Bad intersections omitted\n");
  }

  auto it = find_if(Xs.begin(), Xs.end(), [](const pIntersection_t& i) {
    return i->kind == CSpatialKind::HARD_EDGE;
  });

  if (it != Xs.end()) {
    Xs.erase(++it, Xs.end());
  }

  return Xs;
}

//===========================================
// SpatialSystem::entitiesAlong3dRay
//===========================================
list<pIntersection_t> SpatialSystem::entitiesAlong3dRay(const CZone& zone, const Point& pos,
  double height, const Vec2f& dir, double vAngle, const Matrix& matrix) const {

  list<pIntersection_t> intersections = entitiesAlongRay(zone, pos, dir, matrix);

  for (auto it = intersections.begin(); it != intersections.end();) {
    Intersection& X = **it;

    double y = height + X.distanceFromOrigin * tan(vAngle);

    if (!isBetween(y, X.heightRanges.first.a, X.heightRanges.first.b)
      && !isBetween(y, X.heightRanges.second.a, X.heightRanges.second.b)) {

      intersections.erase(it++);
    }
    else {
      X.height = y - X.heightRanges.first.a;
      ++it;
    }
  }

  return intersections;
}

//===========================================
// SpatialSystem::entitiesAlong3dRay
//===========================================
list<pIntersection_t> SpatialSystem::entitiesAlong3dRay(const Vec2f& ray,
  double camSpaceVAngle) const {

  const Camera& camera = sg.player->camera();
  double height = camera.height;
  double vAngle = camSpaceVAngle + camera.vAngle;
  Matrix matrix = camera.matrix().inverse();

  list<pIntersection_t> intersections = entitiesAlong3dRay(getCurrentZone(), Point(0, 0), height,
    ray, vAngle, matrix);

  auto it = find_if(intersections.begin(), intersections.end(), [&](const pIntersection_t& i) {
    return i->entityId == sg.player->body;
  });

  erase(intersections, it);

  return intersections;
}

//===========================================
// entitiesInRadius
//===========================================
static void entitiesInRadius_r(const CZone& searchZone, const CZone& zone, const Circle& circle,
  double heightAboveFloor, set<entityId_t>& entities) {

  const double MAX_VERTICAL_DISTANCE = 20.0;

  for (auto it = searchZone.edges.begin(); it != searchZone.edges.end(); ++it) {
    const CEdge& edge = **it;

    if (overlapsCircle(circle, edge)) {
      entities.insert(edge.entityId());
      entities.insert(searchZone.entityId());

      if (edge.kind == CSpatialKind::HARD_EDGE || edge.kind == CSpatialKind::SOFT_EDGE) {
        for (auto jt = edge.vRects.begin(); jt != edge.vRects.end(); ++jt) {
          const CVRect& vRect = **jt;

          if (overlapsCircle(circle, edge.lseg, vRect)) {
            assert(vRect.zone != nullptr);

            double vRectFloorH = vRect.zone->floorHeight;

            if (edge.kind == CSpatialKind::SOFT_EDGE) {
              const CSoftEdge& se = dynamic_cast<const CSoftEdge&>(edge);
              vRectFloorH = smallest(se.zoneA->floorHeight, se.zoneB->floorHeight);
            }

            double y1 = zone.floorHeight + heightAboveFloor;
            double y2 = vRectFloorH + vRect.pos.y + 0.5 * vRect.size.y;

            if (fabs(y1 - y2) <= MAX_VERTICAL_DISTANCE) {
              entities.insert(vRect.entityId());
            }
          }
        }
      }
    }
  }

  for (auto it = searchZone.vRects.begin(); it != searchZone.vRects.end(); ++it) {
    const CVRect& vRect = **it;

    if (overlapsCircle(circle, vRect)) {
      assert(vRect.zone != nullptr);

      double y1 = zone.floorHeight + heightAboveFloor;
      // TODO: Currently CVRects that are direct children of zones don't have a vertical position.
      // Their y coord corresponds to the x-y coords of the map.
      double y2 = vRect.zone->floorHeight + 0.5 * vRect.size.y;

      if (fabs(y1 - y2) <= MAX_VERTICAL_DISTANCE) {
        entities.insert(vRect.entityId());
      }
    }
  }

  for (auto it = searchZone.hRects.begin(); it != searchZone.hRects.end(); ++it) {
    if (overlapsCircle(circle, **it)) {
      entities.insert((*it)->entityId());
    }
  }

  for (auto it = searchZone.children.begin(); it != searchZone.children.end(); ++it) {
    entitiesInRadius_r(**it, zone, circle, heightAboveFloor, entities);
  }
}

//===========================================
// SpatialSystem::entitiesInRadius
//===========================================
set<entityId_t> SpatialSystem::entitiesInRadius(const CZone& zone, const Point& pos, double radius,
  double heightAboveFloor) const {

  set<entityId_t> entities;
  Circle circle{pos, radius};

  entitiesInRadius_r(nthConstAncestor(getCurrentZone(), MAX_ANCESTOR_SEARCH), zone, circle,
    heightAboveFloor, entities);

  return entities;
}

//===========================================
// SpatialSystem::update
//===========================================
void SpatialSystem::update() {
  buoyancy();
  gravity();
}

//===========================================
// SpatialSystem::hasComponent
//===========================================
bool SpatialSystem::hasComponent(entityId_t entityId) const {
  return m_components.find(entityId) != m_components.end();
}

//===========================================
// SpatialSystem::getComponent
//===========================================
Component& SpatialSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
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

  if (c->parentId == -1) {
    if (c->kind != CSpatialKind::ZONE) {
      EXCEPTION("Component has no parent and is not root zone");
    }

    if (sg.rootZone) {
      EXCEPTION("Root zone already set");
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

    addChildToComponent(*parent, std::move(c));
  }

  m_entityChildren[ptr->entityId()];
  m_components.insert(make_pair(ptr->entityId(), ptr));
}

//===========================================
// SpatialSystem::isRoot
//===========================================
bool SpatialSystem::isRoot(const CSpatial& c) const {
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
// SpatialSystem::removeChildFromComponent
//===========================================
bool SpatialSystem::removeChildFromComponent(CSpatial& parent, const CSpatial& child,
  bool keepAlive) {

  m_entityChildren.at(parent.entityId()).erase(child.entityId());

  switch (parent.kind) {
    case CSpatialKind::ZONE:
      return removeFromZone(sg, dynamic_cast<CZone&>(parent), child, keepAlive);
    case CSpatialKind::HARD_EDGE:
      return removeFromHardEdge(dynamic_cast<CHardEdge&>(parent), child, keepAlive);
    default:
      EXCEPTION("Cannot remove component of kind " << child.kind << " from component of kind "
        << parent.kind);
  };
}

//===========================================
// SpatialSystem::removeEntity_r
//===========================================
void SpatialSystem::removeEntity_r(entityId_t id) {
  m_components.erase(id);

  auto it = m_entityChildren.find(id);
  if (it != m_entityChildren.end()) {
    set<entityId_t>& children = it->second;

    for (entityId_t child : children) {
      removeEntity_r(child);
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
  auto jt = m_components.find(c.parentId);

  if (jt != m_components.end()) {
    CSpatial& parent = *jt->second;
    removeChildFromComponent(parent, c);
  }

  removeEntity_r(id);
}
