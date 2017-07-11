#include <string>
#include <cassert>
#include <sstream>
#include <list>
#include <iterator>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::stringstream;
using std::unique_ptr;
using std::string;
using std::list;
using std::map;


const double PLAYER_STEP_HEIGHT = 16.0;
// World units per second
const double PLAYER_VERTICAL_SPEED = 100.0;
const double PLAYER_FALL_SPEED = 400.0;


//===========================================
// getValue
//===========================================
static const string& getValue(const map<string, string>& m, const string& key) {
  try {
    return m.at(key);
  }
  catch (std::out_of_range& ex) {
    EXCEPTION("No '" << key << "' key in map");
  }
}

//===========================================
// forEachConstRegion
//===========================================
static void forEachConstRegion(const Region& region, std::function<void(const Region&)> fn) {
  fn(region);
  std::for_each(region.children.begin(), region.children.end(),
    [&](const std::unique_ptr<Region>& r) {

    forEachConstRegion(*r, fn);
  });
}

//===========================================
// forEachRegion
//===========================================
static void forEachRegion(Region& region, std::function<void(Region&)> fn) {
  fn(region);
  std::for_each(region.children.begin(), region.children.end(),
    [&](std::unique_ptr<Region>& r) {

    forEachRegion(*r, fn);
  });
}

//===========================================
// constructWalls
//===========================================
static list<Wall*> constructWalls(const parser::Object& obj, Region* region,
  const Matrix& parentTransform) {

  DBG_PRINT("Constructing Walls\n");

  list<Wall*> walls;

  for (unsigned int i = 0; i < obj.path.points.size(); ++i) {
    int j = i - 1;

    if (i == 0) {
      if (obj.path.closed) {
        j = obj.path.points.size() - 1;
      }
      else {
        continue;
      }
    }

    Wall* wall = new Wall;

    wall->lseg.A = obj.path.points[j];
    wall->lseg.B = obj.path.points[i];
    wall->lseg = transform(wall->lseg, parentTransform * obj.transform);

    wall->region = region;
    wall->texture = getValue(obj.dict, "texture");

    walls.push_back(wall);
  }

  return walls;
}

//===========================================
// constructCamera
//===========================================
static Camera* constructCamera(const parser::Object& obj, const Region& region,
  const Matrix& parentTransform) {

  DBG_PRINT("Constructing Camera\n");

  Camera* camera = new Camera;
  camera->setTransform(parentTransform * obj.transform * transformFromTriangle(obj.path));

  camera->height = std::stod(getValue(obj.dict, "height")) + region.floorHeight;

  return camera;
}

//===========================================
// constructSprite
//===========================================
static Sprite* constructSprite(const parser::Object& obj, Region& region,
  const Matrix& parentTransform) {

  DBG_PRINT("Constructing Sprite\n");

  if (getValue(obj.dict, "subtype") == "bad_guy") {
    BadGuy* sprite = new BadGuy;
    Matrix m = transformFromTriangle(obj.path);
    sprite->setTransform(parentTransform * obj.transform * m);
    sprite->region = &region;

    return sprite;
  }
  else if (getValue(obj.dict, "subtype") == "ammo") {
    Ammo* sprite = new Ammo;
    Matrix m = transformFromTriangle(obj.path);
    sprite->setTransform(parentTransform * obj.transform * m);
    sprite->region = &region;

    return sprite;
  }

  EXCEPTION("Error constructing sprite of unknown type");
}

//===========================================
// constructJoiningEdges
//===========================================
static list<JoiningEdge*> constructJoiningEdges(const parser::Object& obj, Region* region,
  const Matrix& parentTransform) {

  DBG_PRINT("Constructing JoiningEdges\n");

  list<JoiningEdge*> joiningEdges;

  for (unsigned int i = 0; i < obj.path.points.size(); ++i) {
    int j = i - 1;

    if (i == 0) {
      if (obj.path.closed) {
        j = obj.path.points.size() - 1;
      }
      else {
        continue;
      }
    }

    JoiningEdge* je = new JoiningEdge;

    je->lseg.A = obj.path.points[j];
    je->lseg.B = obj.path.points[i];
    je->lseg = transform(je->lseg, parentTransform * obj.transform);

    if (contains<string>(obj.dict, "top_texture")) {
      je->topTexture = getValue(obj.dict, "top_texture");
    }
    if (contains<string>(obj.dict, "bottom_texture")) {
      je->bottomTexture = getValue(obj.dict, "bottom_texture");
    }

    joiningEdges.push_back(je);
  }

  return joiningEdges;
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
// combine
//===========================================
static JoiningEdge* combine(const JoiningEdge& je1, const JoiningEdge& je2) {
  JoiningEdge* je = new JoiningEdge(je1);
  je->mergeIn(je2);
  return je;
}

//===========================================
// connectSubregions_r
//===========================================
static void connectSubregions_r(Scene& scene, Region& region) {
  if (region.children.size() == 0) {
    return;
  }

  for (auto it = region.children.begin(); it != region.children.end(); ++it) {
    Region& r = **it;
    connectSubregions_r(scene, r);

    for (auto jt = r.edges.begin(); jt != r.edges.end(); ++jt) {
      if ((*jt)->kind == EdgeKind::JOINING_EDGE) {
        JoiningEdge* je = dynamic_cast<JoiningEdge*>(*jt);
        assert(je != nullptr);

        bool hasTwin = false;
        forEachRegion(region, [&](Region& r_) {
          if (!hasTwin) {
            if (&r_ != &r) {
              for (auto lt = r_.edges.begin(); lt != r_.edges.end(); ++lt) {
                if ((*lt)->kind == EdgeKind::JOINING_EDGE) {
                  JoiningEdge* other = dynamic_cast<JoiningEdge*>(*lt);
                  assert(other != nullptr);

                  if (je == other) {
                    hasTwin = true;
                    break;
                  }

                  if (areTwins(*je, *other)) {
                    hasTwin = true;

                    JoiningEdge* combined = combine(*je, *other);
                    combined->regionA = &r;
                    combined->regionB = &r_;

                    delete je;
                    delete other;

                    *jt = combined;
                    *lt = combined;

                    scene.edges.push_back(pEdge_t(combined));
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
// constructRegion_r
//===========================================
static Region* constructRegion_r(Scene& scene, const parser::Object& obj,
  const Matrix& parentTransform) {

  DBG_PRINT("Constructing Region\n");

  Region* region = new Region;

  try {
    if (getValue(obj.dict, "type") != "region") {
      EXCEPTION("Object is not of type region");
    }

    if (obj.path.points.size() > 0) {
      EXCEPTION("Region has unexpected path");
    }

    Matrix transform = parentTransform * obj.transform;

    if (contains<string>(obj.dict, "has_ceiling")) {
      string s = getValue(obj.dict, "has_ceiling");
      if (s == "true") {
        region->hasCeiling = true;
      }
      else if (s == "false") {
        region->hasCeiling = false;
      }
      else {
        EXCEPTION("has_ceiling must be either 'true' or 'false'");
      }
    }

    region->floorHeight = contains<string>(obj.dict, "floor_height") ?
      std::stod(getValue(obj.dict, "floor_height")) : scene.defaultFloorHeight;

    region->ceilingHeight = contains<string>(obj.dict, "ceiling_height") ?
      std::stod(getValue(obj.dict, "ceiling_height")) : scene.defaultCeilingHeight;

    region->floorTexture = contains<string>(obj.dict, "floor_texture") ?
      getValue(obj.dict, "floor_texture") : scene.defaultFloorTexture;

    region->ceilingTexture = contains<string>(obj.dict, "ceiling_texture") ?
      getValue(obj.dict, "ceiling_texture") : scene.defaultCeilingTexture;

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      const parser::Object& child = **it;
      string type = getValue(child.dict, "type");

      if (type == "region") {
        region->children.push_back(pRegion_t(constructRegion_r(scene, child, transform)));
      }
      else if (type == "wall") {
        list<Wall*> walls = constructWalls(child, region, transform);
        for (auto jt = walls.begin(); jt != walls.end(); ++jt) {
          scene.edges.push_back(pEdge_t(*jt));
          region->edges.push_back(*jt);
        }
      }
      else if (type == "joining_edge") {
        list<JoiningEdge*> joiningEdges = constructJoiningEdges(child, region, transform);
        for (auto jt = joiningEdges.begin(); jt != joiningEdges.end(); ++jt) {
          region->edges.push_back(*jt);
        }
      }
      else if (type == "sprite") {
        region->sprites.push_back(pSprite_t(constructSprite(child, *region, transform)));
      }
      else if (type == "camera") {
        if (scene.camera) {
          EXCEPTION("Camera already exists");
        }
        scene.camera.reset(constructCamera(child, *region, transform));
        scene.currentRegion = region;
      }
    }
  }
  catch (Exception& ex) {
    delete region;
    ex.prepend("Error constructing region; ");
    throw ex;
  }
  catch (const std::exception& ex) {
    delete region;
    EXCEPTION("Error constructing region; " << ex.what());
  }

  return region;
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
// Scene::Scene
//===========================================
Scene::Scene(const string& mapFilePath, double frameRate) {
  m_frameRate = frameRate;

  defaultFloorHeight = 0;
  defaultCeilingHeight = 100;
  defaultFloorTexture = "cracked_mud";
  defaultCeilingTexture = "grey_stone";

  list<parser::pObject_t> objects;
  parser::parse(mapFilePath, objects);

  assert(objects.size() == 1);
  addObject(**objects.begin());

  if (!camera) {
    EXCEPTION("Scene must contain a camera");
  }

  connectSubregions_r(*this, *rootRegion);

  viewport.x = 10.0 * 320.0 / 240.0; // TODO: Read from map file
  viewport.y = 10.0;

  camera->F = computeF(viewport.x, camera->hFov);

  textures["default"] = Texture{QImage("data/default.png"), Size(100, 100)};
  textures["light_bricks"] = Texture{QImage("data/light_bricks.png"), Size(100, 100)};
  textures["dark_bricks"] = Texture{QImage("data/dark_bricks.png"), Size(100, 100)};
  textures["cracked_mud"] = Texture{QImage("data/cracked_mud.png"), Size(100, 100)};
  textures["dirt"] = Texture{QImage("data/dirt.png"), Size(100, 100)};
  textures["crate"] = Texture{QImage("data/crate.png"), Size(30, 30)};
  textures["grey_stone"] = Texture{QImage("data/grey_stone.png"), Size(100, 100)};
  textures["stone_slabs"] = Texture{QImage("data/stone_slabs.png"), Size(100, 100)};
  textures["ammo"] = Texture{QImage("data/ammo.png"), Size(100, 100)};
  textures["bad_guy"] = Texture{QImage("data/bad_guy.png"), Size(100, 100)};
  textures["sky"] = Texture{QImage("data/sky.png"), Size()};
}

//===========================================
// Scene::rotateCamera
//===========================================
void Scene::rotateCamera(double da) {
  camera->angle += da;
}

//===========================================
// getDelta
//===========================================
static Vec2f getDelta(const Region& region, const Point& camPos, double playerH, double radius,
  const Vec2f& dv) {

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

            //assert(&region == je.regionA || &region == je.regionB);
            Region* nextRegion = je.regionA == &region ? je.regionB : je.regionA;

            if (nextRegion->floorHeight - playerH <= PLAYER_STEP_HEIGHT) {
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
// Scene::translateCamera
//===========================================
void Scene::translateCamera(const Vec2f& dir) {
  Camera& cam = *camera;

  Vec2f dv(cos(cam.angle) * dir.x - sin(cam.angle) * dir.y,
    sin(cam.angle) * dir.x + cos(cam.angle) * dir.y);

  double radius = 5.0;
  double playerH = cam.height - m_player.height;

  dv = getDelta(*currentRegion, cam.pos, playerH, radius, dv);
  Circle circle{cam.pos + dv, radius};

  bool abortLoop = false;
  forEachConstRegion(*currentRegion, [&](const Region& region) {
    if (abortLoop) {
      return;
    }

    int nIntersections = 0;
    double nearestX = 999999.9;
    Region* nextRegion = nullptr;
    double dy = 0;
    Point p;

    for (auto it = region.edges.begin(); it != region.edges.end(); ++it) {
      const Edge& edge = **it;

      if (lineSegmentCircleIntersect(circle, edge.lseg)) {
        assert(edge.kind == EdgeKind::JOINING_EDGE);
        const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

        assert(currentRegion == je.regionA || currentRegion == je.regionB);
        Region* nextRegion_ = je.regionA == currentRegion ? je.regionB : je.regionA;

        double floorH = currentRegion->floorHeight;
        double floorDiff = nextRegion_->floorHeight - floorH;
        double playerH = cam.height - m_player.height;
        double stepH = nextRegion_->floorHeight - playerH;

        if (stepH <= PLAYER_STEP_HEIGHT) {
          LineSegment ray(cam.pos, cam.pos + dv);
          Point p_;
          bool crossesLine = lineSegmentIntersect(ray, edge.lseg, p_);

          if (crossesLine) {
            ++nIntersections;

            double dist = distance(cam.pos, p_);
            if (dist < nearestX) {
              nextRegion = nextRegion_;
              dy = floorDiff;
              p = p_;

              nearestX = dist;
            }
          }
        }
      }
    }

    if (nIntersections > 0) {
      currentRegion = nextRegion;
      int frames = 0;

      if (dy < 0 && dy < -PLAYER_STEP_HEIGHT) {
        frames = (fabs(dy) / PLAYER_FALL_SPEED) * m_frameRate;
      }
      else {
        frames = (fabs(dy) / PLAYER_VERTICAL_SPEED) * m_frameRate;
      }

      if (frames > 0) {
        double dy_ = dy / frames;
        int i = 0;

        addTween(Tween{[&, dy_, i, frames]() mutable -> bool {
          cam.height += dy_;
          return ++i < frames;
        }, []() {}});
      }

      cam.pos = p;
      dv = dv * 0.00001;
      abortLoop = true;
    }
  });

  double dy = 10.0;
  double frames = 20;
  double dy_ = dy / frames;
  int i = 0;
  addTween(Tween{[&, dy_, i, frames]() mutable -> bool {
    if (i < frames / 2) {
      cam.height += dy_;
    }
    else {
      cam.height -= dy_;
    }
    return ++i < frames;
  }, []() {}}, "playerBounce");

  cam.pos = cam.pos + dv;
}

//===========================================
// Scene::jump
//===========================================
void Scene::jump() {
  if (m_player.isJumping) {
    return;
  }

  Camera& cam = *camera;

  double dy = 80.0;
  double frames = 30;
  double dy_ = dy / frames;
  int i = 0;
  addTween(Tween{[&, dy_, i, frames]() mutable -> bool {
    if (i < frames / 2) {
      cam.height += dy_;
    }
    else {
      cam.height -= dy_;
    }
    return ++i < frames;
  }, []() {}}, "playerJump");
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
}

//===========================================
// Scene::addObject
//===========================================
void Scene::addObject(const parser::Object& obj) {
  if (getValue(obj.dict, "type") == "region") {
    if (rootRegion) {
      EXCEPTION("Root region already exists");
    }

    Matrix m;
    rootRegion.reset(constructRegion_r(*this, obj, m));
  }
}
