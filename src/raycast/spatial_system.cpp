#include <string>
#include <cassert>
#include <sstream>
#include <ostream>
#include <list>
#include <iterator>
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


static const double GRID_CELL_SIZE = 25.0;


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
// getNextZone
//===========================================
static CZone* getNextZone(const CZone& current, const CSoftEdge& se) {
  return se.zoneA == &current ? se.zoneB : se.zoneA;
}

//===========================================
// canStepAcross
//===========================================
static bool canStepAcross(const CVRect& body, double height, const CSoftEdge& se) {
  CZone* nextZone = getNextZone(*body.zone, se);

  bool canStep = nextZone->floorHeight - height <= PLAYER_STEP_HEIGHT;
  bool hasHeadroom = height + body.size.y < nextZone->ceilingHeight;

  return canStep && hasHeadroom;
}

//===========================================
// intersectHardEdge
//===========================================
static bool intersectHardEdge(const CZone& zone, const Circle& circle, const CVRect& body,
  double height) {

  bool b = false;

  forEachConstZone(zone, [&](const CZone& r) {
    if (b) {
      return;
    }

    for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
      const CEdge& edge = **it;

      if (lineSegmentCircleIntersect(circle, edge.lseg)) {
        if (edge.kind != CSpatialKind::SOFT_EDGE) {
          std::cout << "1\n";
          b = true;
          break;
        }

        const CSoftEdge& se = dynamic_cast<const CSoftEdge&>(edge);

        if (!canStepAcross(body, height, se)) {
          std::cout << "2\n";
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
static Vec2f getDelta(const CZone& zone, const Point& camPos, const CVRect& body, double height,
  double radius, const Vec2f& dv) {

  Circle circle{camPos + dv, radius};
  LineSegment ray(camPos, camPos + dv);

  Vec2f dv_ = dv;

  assert(zone.parent != nullptr);

  bool abortLoop = false;
  forEachConstZone(*zone.parent, [&](const CZone& r) {
    if (abortLoop == false) {
      for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
        const CEdge& edge = **it;

        // If moving by dv will intersect something
        if (lineSegmentCircleIntersect(circle, edge.lseg)) {
          Point p = lineIntersect(ray.line(), edge.lseg.line());
          // If we're moving away from the wall
          if (distance(camPos + dv_ * 0.00001, p) > distance(camPos, p)) {
            continue;
          }

          if (edge.kind != CSpatialKind::SOFT_EDGE ||
            !canStepAcross(body, height, dynamic_cast<const CSoftEdge&>(edge))) {

            // Erase component in direction of wall
            Matrix m(-atan(edge.lseg.line().m), Vec2f());
            dv_ = m * dv;
            dv_.y = 0;
            dv_ = m.inverse() * dv_;
          }

          if (intersectHardEdge(*zone.parent, Circle{camPos + dv_, radius}, body, height)) {
            DBG_PRINT_VAR(dv_);
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
  }, [](long, double, double) {}}, "playerBounce");
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
  if (event.name == "playerActivate") {
    GameEvent e("playerActivateEntity");
    m_entityManager.broadcastEvent(e, entitiesInRadius(sg.player->pos(),
      sg.player->activationRadius));
  }
}

//===========================================
// SpatialSystem::~SpatialSystem
//===========================================
SpatialSystem::~SpatialSystem() {}

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
// SpatialSystem::moveEntity
//===========================================
void SpatialSystem::moveEntity(entityId_t id, Vec2f dv, double heightAboveFloor) {
  auto it = m_components.find(id);
  if (it != m_components.end()) {
    CSpatial& c = *it->second;

    // Currently, only VRects can be moved
    if (c.kind == CSpatialKind::V_RECT) {
      CVRect& body = dynamic_cast<CVRect&>(c);
      CZone& currentZone = *body.zone;

      // TODO
      double radius = 10.0; //body.size.x * 0.5;

      dv = getDelta(currentZone, body.pos, body, currentZone.floorHeight + heightAboveFloor, radius,
        dv);

      Circle circle{body.pos + dv, radius};

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
          const CEdge& edge = **it;

          if (lineSegmentCircleIntersect(circle, edge.lseg)) {
            assert(edge.kind == CSpatialKind::SOFT_EDGE);

            const CSoftEdge& se = dynamic_cast<const CSoftEdge&>(edge);

            LineSegment ray(body.pos, body.pos + dv);
            Point p_;
            bool crossesLine = lineSegmentIntersect(ray, edge.lseg, p_);

            if (crossesLine) {
              ++nIntersections;

              double dist = distance(body.pos, p_);
              if (dist < nearestX) {
                nextZone = getNextZone(currentZone, se);
                p = p_;

                nearestX = dist;
              }
            }
          }
        }

        if (nIntersections > 0) {
          body.zone = nextZone;
          body.pos = p;
          dv = dv * 0.00001;
          abortLoop = true;

          crossZones(sg, id, currentZone.entityId(), nextZone->entityId());

          m_entityManager.broadcastEvent(EChangedZone(id, currentZone.entityId(),
            nextZone->entityId()));
        }
      });

      body.pos = body.pos + dv;
    }
  }
}

//===========================================
// SpatialSystem::movePlayer
//===========================================
void SpatialSystem::movePlayer(const Vec2f& v) {
  Player& player = *sg.player;
  const Camera& cam = player.camera();

  Vec2f dv(cos(cam.angle) * v.x - sin(cam.angle) * v.y,
    sin(cam.angle) * v.x + cos(cam.angle) * v.y);

  moveEntity(player.body.entityId(), dv, player.feetHeight() - player.body.zone->floorHeight);
  player.setPosition(player.body.zone->entityId(), player.body.pos);

  playerBounce(*this, m_timeService, m_frameRate);

  Point pos = player.pos();
  Vec2i cell(pos.x / GRID_CELL_SIZE, pos.y / GRID_CELL_SIZE);

  if (cell != m_playerCell) {
    m_playerCell = cell;

    GameEvent e("playerMove");
    m_entityManager.broadcastEvent(e, entitiesInRadius(player.pos(), player.collectionRadius));
  }
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
// SpatialSystem::buoyancy
//===========================================
void SpatialSystem::buoyancy() {
  CZone& currentZone = getCurrentZone();

  if (sg.player->feetHeight() + 0.1 < currentZone.floorHeight) {
    double dy = 150.0 / m_frameRate;
    sg.player->changeHeight(currentZone, dy);
  }
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
// entitiesInRadius
//===========================================
static void entitiesInRadius_r(const CZone& zone, const Circle& circle,
  set<entityId_t>& entities) {

  for (auto it = zone.edges.begin(); it != zone.edges.end(); ++it) {
    if (overlapsCircle(circle, **it)) {
      entities.insert((*it)->entityId());
      entities.insert(zone.entityId());

      if ((*it)->kind == CSpatialKind::HARD_EDGE) {
        const CHardEdge& he = dynamic_cast<const CHardEdge&>(**it);

        for (auto jt = he.vRects.begin(); jt != he.vRects.end(); ++jt) {
          if (overlapsCircle(circle, he.lseg, **jt)) {
            entities.insert((*jt)->entityId());
          }
        }
      }
    }
  }

  for (auto it = zone.vRects.begin(); it != zone.vRects.end(); ++it) {
    if (overlapsCircle(circle, **it)) {
      entities.insert((*it)->entityId());
    }
  }

  for (auto it = zone.hRects.begin(); it != zone.hRects.end(); ++it) {
    if (overlapsCircle(circle, **it)) {
      entities.insert((*it)->entityId());
    }
  }

  for (auto it = zone.children.begin(); it != zone.children.end(); ++it) {
    entitiesInRadius_r(**it, circle, entities);
  }
}

//===========================================
// similar
//===========================================
static bool similar(const LineSegment& l1, const LineSegment& l2) {
  double delta = 2.0;
  return (distance(l1.A, l2.A) <= delta && distance(l1.B, l2.B) <= delta)
    || (distance(l1.A, l2.B) <= delta && distance(l1.B, l2.A) <= delta);
}

//===========================================
// areTwins
//===========================================
static bool areTwins(const CSoftEdge& se1, const CSoftEdge& se2) {
  return similar(se1.lseg, se2.lseg);
}

//===========================================
// connectSubzones
//===========================================
void connectSubzones(CZone& zone) {
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
                    se->zoneA = other->zoneA = &r;
                    se->zoneB = other->zoneB = &r_;
                    se->lseg = other->lseg;

                    break;
                  }
                }
              }
            }
          }
        });

        if (!hasTwin) {
          se->zoneA = &r;
          se->zoneB = r.parent;
        }
      }
    }
  });
}

//===========================================
// SpatialSystem::connectZones
//===========================================
void SpatialSystem::connectZones() {
  connectSubzones(*sg.rootZone);
}

//===========================================
// findIntersections_r
//===========================================
void findIntersections_r(const Point& point, const Vec2f& dir, const Matrix& matrix,
  const CZone& zone, list<pIntersection_t>& intersections, set<const CZone*>& visitedZones,
  set<entityId_t>& visitedJoins) {

  Matrix invMatrix = matrix.inverse();

  LineSegment ray(point, 10000.0 * dir);
  visitedZones.insert(&zone);

  for (auto it = zone.children.begin(); it != zone.children.end(); ++it) {
    if (visitedZones.find(it->get()) == visitedZones.end()) {
      findIntersections_r(point, dir, matrix, **it, intersections, visitedZones, visitedJoins);
    }
  }

  for (auto it = zone.vRects.begin(); it != zone.vRects.end(); ++it) {
    CVRect& vRect = **it;

    Point pos = matrix * vRect.pos;
    double w = vRect.size.x;
    LineSegment lseg(Point(pos.x, pos.y - 0.5 * w), Point(pos.x, pos.y + 0.5 * w));

    Point pt;
    if (lineSegmentIntersect(ray, lseg, pt)) {
      Intersection* X = new Intersection(CSpatialKind::V_RECT);
      intersections.push_back(pIntersection_t(X));
      X->entityId = vRect.entityId();
      X->point_rel = pt;
      X->point_wld = invMatrix * pt;
      X->distanceFromOrigin = pt.x;
      X->distanceAlongTarget = distance(lseg.A, pt);
    }
  }

  for (auto it = zone.edges.begin(); it != zone.edges.end(); ++it) {
    CEdge& edge = **it;
    LineSegment lseg = transform(edge.lseg, matrix);

    Point pt;
    if (lineSegmentIntersect(ray, lseg, pt)) {
      Intersection* X = new Intersection(edge.kind);
      X->entityId = edge.entityId();
      X->point_rel = pt;
      X->point_wld = invMatrix * pt;
      X->distanceFromOrigin = pt.x;
      X->distanceAlongTarget = distance(lseg.A, pt);

      if (edge.kind == CSpatialKind::HARD_EDGE) {
        intersections.push_back(pIntersection_t(X));
      }
      else if (edge.kind == CSpatialKind::SOFT_EDGE) {
        CSoftEdge& softEdge = dynamic_cast<CSoftEdge&>(edge);
        const CZone& next = softEdge.zoneA == &zone ? *softEdge.zoneB : *softEdge.zoneA;

        auto pX = pIntersection_t(X);
        if (visitedJoins.find(softEdge.joinId) == visitedJoins.end()) {
          intersections.push_back(std::move(pX));
          visitedJoins.insert(softEdge.joinId);
        }

        if (visitedZones.find(&next) == visitedZones.end()) {
          findIntersections_r(point, dir, matrix, next, intersections, visitedZones,
            visitedJoins);
        }
      }
      else {
        EXCEPTION("Unexpected intersection type");
      }
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
    return i->entityId == sg.player->body.entityId();
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
  set<const CZone*> visitedZones;
  set<entityId_t> visitedJoins;
  findIntersections_r(pos, dir, matrix, zone, intersections, visitedZones, visitedJoins);

  intersections.sort([](const pIntersection_t& a, const pIntersection_t& b) {
    return a->distanceFromOrigin < b->distanceFromOrigin;
  });

  auto it = find_if(intersections.begin(), intersections.end(), [](const pIntersection_t& i) {
    return i->kind == CSpatialKind::HARD_EDGE;
  });

  if (it != intersections.end()) {
    intersections.erase(++it, intersections.end());
  }

  return intersections;
}

//===========================================
// SpatialSystem::getHeightRangeForEntity
//===========================================
pair<Range, Range> SpatialSystem::getHeightRangeForEntity(entityId_t id) const {
  auto it = m_components.find(id);
  if (it != m_components.end()) {
    const CSpatial& c = *it->second;

    switch (c.kind) {
      case CSpatialKind::V_RECT: {
        const CVRect& vRect = dynamic_cast<const CVRect&>(c);
        return make_pair(Range(vRect.zone->floorHeight, vRect.zone->floorHeight + vRect.size.y),
          Range(0, 0));
      }
      case CSpatialKind::SOFT_EDGE: {
        const CSoftEdge& se = dynamic_cast<const CSoftEdge&>(c);
        return make_pair(Range(se.zoneA->floorHeight, se.zoneB->floorHeight),
          Range(se.zoneA->ceilingHeight, se.zoneB->ceilingHeight));
      }
      // ...
      default:
        return make_pair(Range(-10000, 10000), Range(0, 0));
    }
  }
  else {
    return make_pair(Range(-10000, 10000), Range(0, 0));
  }
}

//===========================================
// SpatialSystem::entitiesAlong3dRay
//===========================================
list<pIntersection_t> SpatialSystem::entitiesAlong3dRay(const CZone& zone, const Point& pos,
  double height, const Vec2f& dir, double vAngle, const Matrix& matrix) const {

  list<pIntersection_t> intersections = entitiesAlongRay(zone, pos, dir, matrix);

  for (auto it = intersections.begin(); it != intersections.end();) {
    const Intersection& X = **it;

    double y = height + X.distanceFromOrigin * tan(vAngle);
    auto ranges = getHeightRangeForEntity(X.entityId);

    if (!isBetween(y, ranges.first.a, ranges.first.b)
      && !isBetween(y, ranges.second.a, ranges.second.b)) {

      intersections.erase(it++);
    }
    else {
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
    return i->entityId == sg.player->body.entityId();
  });

  erase(intersections, it);

  return intersections;
}

//===========================================
// SpatialSystem::entitiesInRadius
//===========================================
set<entityId_t> SpatialSystem::entitiesInRadius(const Point& pos, double radius) const {
  set<entityId_t> entities;
  Circle circle{pos, radius};

  forEachConstZone(*getCurrentZone().parent, [&](const CZone& zone) {
    entitiesInRadius_r(zone, circle, entities);
  });

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
static void addChildToComponent(SceneGraph& sg, CSpatial& parent, pCSpatial_t child) {
  switch (parent.kind) {
    case CSpatialKind::ZONE:
      addToZone(sg, dynamic_cast<CZone&>(parent), std::move(child));
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
    default:
      EXCEPTION("Cannot add component of kind " << child.kind << " to zone");
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
    default:
      EXCEPTION("Cannot remove component of kind " << child.kind << " from HardEdge");
  }

  return found;
}

//===========================================
// removeChildFromComponent
//===========================================
static bool removeChildFromComponent(SceneGraph& sg, CSpatial& parent, const CSpatial& child,
  bool keepAlive = false) {

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
// SpatialSystem::crossZones
//===========================================
void SpatialSystem::crossZones(SceneGraph& sg, entityId_t entityId, entityId_t oldZoneId,
  entityId_t newZoneId) {

  auto it = m_components.find(entityId);
  if (it != m_components.end()) {
    CSpatial& c = *it->second;
    CZone& oldZone = dynamic_cast<CZone&>(getComponent(oldZoneId));
    CZone& newZone = dynamic_cast<CZone&>(getComponent(newZoneId));

    if (removeFromZone(sg, oldZone, c, true)) {
      addChildToComponent(sg, newZone, pCSpatial_t(&c));
    }
  }
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
