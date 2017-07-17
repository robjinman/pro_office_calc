#include <string>
#include <cassert>
#include <sstream>
#include <list>
#include <iterator>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_object_factory.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/tween_curves.hpp"
#include "exception.hpp"
#include "utils.hpp"
#include "event_system.hpp"


using std::stringstream;
using std::unique_ptr;
using std::function;
using std::string;
using std::list;
using std::map;


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
// intersectWall
//===========================================
static bool intersectWall(const Region& region, const Circle& circle) {
  bool b = false;

  forEachConstRegion(region, [&](const Region& r) {
    if (!b) {
      for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
        const Edge& edge = **it;

        if (edge.kind == EdgeKind::JOINING_EDGE) {
          const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

          //assert(&region == je.regionA || &region == je.regionB);
          Region* nextRegion = je.regionA == &region ? je.regionB : je.regionA;

          if (nextRegion->floorHeight - region.floorHeight <= PLAYER_STEP_HEIGHT) {
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
//===========================================
static Vec2f getDelta(const Region& region, const Point& camPos, const Player& player,
  double radius, const Vec2f& dv) {

  Circle circle{camPos + dv, radius};
  LineSegment ray(camPos, camPos + dv);

  bool collision = false;
  Vec2f dv_ = dv;

  forEachConstRegion(region, [&](const Region& r) {
    if (collision == false) {
      for (auto it = r.edges.begin(); it != r.edges.end(); ++it) {
        const Edge& edge = **it;

        if (lineSegmentCircleIntersect(circle, edge.lseg)) {
          Point p = lineIntersect(ray.line(), edge.lseg.line());
          if (distance(camPos + dv_ * 0.00001, p) > distance(camPos, p)) {
            continue;
          }

          if (edge.kind == EdgeKind::JOINING_EDGE) {
            const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

            assert(&region == je.regionA || &region == je.regionB);
            Region* nextRegion = je.regionA == &region ? je.regionB : je.regionA;

            bool canStep = nextRegion->floorHeight - player.feetHeight() <= PLAYER_STEP_HEIGHT;
            bool hasHeadroom = player.headHeight() < nextRegion->ceilingHeight;

            if (canStep && hasHeadroom) {
              continue;
            }
          }

          Matrix m(-atan(edge.lseg.line().m), Vec2f());
          dv_ = m * dv;
          dv_.y = 0;
          dv_ = m.inverse() * dv_;

          if (intersectWall(region, Circle{camPos + dv_, radius})) {
            dv_ = Vec2f(0, 0);
          }
          else {
            collision = true;
            break;
          }
        }
      }
    }
  });

  return dv_;
}

//===========================================
// Scene::Scene
//===========================================
Scene::Scene(EventSystem& eventSystem, const string& mapFilePath, double frameRate)
  : m_eventSystem(eventSystem) {

  m_frameRate = frameRate;

  list<parser::pObject_t> objects;
  parser::parse(mapFilePath, objects);

  assert(objects.size() == 1);
  constructRootRegion(sg, m_behaviourSystem, **objects.begin());

  m_eventIds.push_back(m_eventSystem.listen("", [&](const Event& e) {
    m_behaviourSystem.handleEvent(e);
  }));
}

//===========================================
// Scene::~Scene
//===========================================
Scene::~Scene() {
  for (auto it = m_eventIds.begin(); it != m_eventIds.end(); ++it) {
    m_eventSystem.forget(*it);
  }
}

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

  dv = getDelta(*sg.currentRegion, cam.pos, *sg.player, radius, dv);
  Circle circle{cam.pos + dv, radius};

  bool abortLoop = false;
  forEachConstRegion(*sg.currentRegion, [&](const Region& region) {
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
        assert(edge.kind == EdgeKind::JOINING_EDGE);
        const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

        //assert(sg.currentRegion == je.regionA || sg.currentRegion == je.regionB);
        Region* nextRegion_ = je.regionA == sg.currentRegion ? je.regionB : je.regionA;
        double stepH = nextRegion_->floorHeight - sg.player->feetHeight();

        if (stepH <= PLAYER_STEP_HEIGHT) {
          LineSegment ray(cam.pos, cam.pos + dv);
          Point p_;
          bool crossesLine = lineSegmentIntersect(ray, edge.lseg, p_);

          if (crossesLine) {
            ++nIntersections;

            double dist = distance(cam.pos, p_);
            if (dist < nearestX) {
              nextRegion = nextRegion_;
              p = p_;

              nearestX = dist;
            }
          }
        }
      }
    }

    if (nIntersections > 0) {
      sg.currentRegion = nextRegion;
      sg.player->setPosition(p);
      dv = dv * 0.00001;
      abortLoop = true;
    }
  });

  sg.player->move(dv);

  double dy = 5.0;
  double frames = 20;
  double dy_ = dy / (frames / 2);
  int i = 0;
  addTween(Tween{[&, dy_, i, frames]() mutable -> bool {
    if (i < frames / 2) {
      sg.player->changeTallness(dy_);
    }
    else {
      sg.player->changeTallness(-dy_);
    }
    return ++i < frames;
  }, []() {}}, "playerBounce");
}

//===========================================
// Scene::jump
//===========================================
void Scene::jump() {
  if (!sg.player->aboveGround(*sg.currentRegion)) {
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
  if (fabs(sg.player->vVelocity) > 0.001 || sg.player->aboveGround(*sg.currentRegion)) {
    double a = -600.0;
    double dt = 1.0 / m_frameRate;
    double dv = dt * a;
    sg.player->vVelocity += dv;
    double dy = sg.player->vVelocity * dt;

    sg.player->changeHeight(*sg.currentRegion, dy);

    if (!sg.player->aboveGround(*sg.currentRegion)) {
      sg.player->vVelocity = 0;
    }
  }
}

//===========================================
// Scene::buoyancy
//===========================================
void Scene::buoyancy() {
  if (sg.player->feetHeight() + 0.1 < sg.currentRegion->floorHeight) {
    double dy = 150.0 / m_frameRate;
    sg.player->changeHeight(*sg.currentRegion, dy);
  }
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

  m_behaviourSystem.update();
}
