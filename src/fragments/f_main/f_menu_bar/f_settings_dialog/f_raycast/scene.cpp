#include <string>
#include <sstream>
#include <list>
#include <iterator>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::unique_ptr;
using std::string;
using std::list;


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
    wall->texture = obj.dict.at("texture");

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

  camera->height = std::stod(obj.dict.at("height")) + region.floorHeight;

  return camera;
}

//===========================================
// constructSprite
//===========================================
static Sprite* constructSprite(const parser::Object& obj, Region& region,
  const Matrix& parentTransform) {

  DBG_PRINT("Constructing Sprite\n");

  if (obj.dict.at("subtype") == "bad_guy") {
    BadGuy* sprite = new BadGuy;
    Matrix m = transformFromTriangle(obj.path);
    sprite->setTransform(parentTransform * obj.transform * m);
    sprite->region = &region;

    return sprite;
  }
  else if (obj.dict.at("subtype") == "ammo") {
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
      je->topTexture = obj.dict.at("top_texture");
    }
    if (contains<string>(obj.dict, "bottom_texture")) {
      je->bottomTexture = obj.dict.at("bottom_texture");
    }

    joiningEdges.push_back(je);
  }

  return joiningEdges;
}

//===========================================
// similar
//===========================================
static bool similar(const LineSegment& l1, const LineSegment& l2) {
  double delta = 0.1;
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
    connectSubregions_r(scene, **it);

    for (auto jt = (*it)->edges.begin(); jt != (*it)->edges.end(); ++jt) {
      if ((*jt)->kind == EdgeKind::JOINING_EDGE) {
        JoiningEdge* je = dynamic_cast<JoiningEdge*>(*jt);

        bool hasTwin = false;
        for (auto kt = region.children.begin(); kt != region.children.end(); ++kt) {
          if (kt->get() == &region) {
            continue;
          }

          for (auto lt = (*kt)->edges.begin(); lt != (*kt)->edges.end(); ++lt) {
            if ((*lt)->kind == EdgeKind::JOINING_EDGE) {
              JoiningEdge* other = dynamic_cast<JoiningEdge*>(*lt);

              if (je == other) {
                hasTwin = true;
                break;
              }

              if (areTwins(*je, *other)) {
                hasTwin = true;

                JoiningEdge* combined = combine(*je, *other);
                combined->regionA = it->get();
                combined->regionB = kt->get();

                delete je;
                delete other;

                *jt = combined;
                *lt = combined;

                scene.edges.push_back(pEdge_t(combined));
                break;
              }
            }
          }

          if (hasTwin) {
            break;
          }
        }

        if (!hasTwin) {
          EXCEPTION("Could not find twin for JoiningEdge");
        }
      }
    }
  }
}

//===========================================
// constructRegion_r
//===========================================
static Region* constructRegion_r(Scene& scene, const parser::Object& obj,
  const Matrix& parentTransform) {

  DBG_PRINT("Constructing Region\n");

  Region* region = new Region;

  try {
    if (obj.dict.at("type") != "region") {
      EXCEPTION("Object is not of type region");
    }

    if (obj.path.points.size() > 0) {
      EXCEPTION("Region has unexpected path");
    }

    Matrix transform = parentTransform * obj.transform;

    region->floorHeight = contains<string>(obj.dict, "floor_height") ?
      std::stod(obj.dict.at("floor_height")) : scene.defaultFloorHeight;

    region->ceilingHeight = contains<string>(obj.dict, "ceiling_height") ?
      std::stod(obj.dict.at("ceiling_height")) : scene.defaultCeilingHeight;

    region->floorTexture = contains<string>(obj.dict, "floor_texture") ?
      obj.dict.at("floor_texture") : scene.defaultFloorTexture;

    region->ceilingTexture = contains<string>(obj.dict, "ceiling_texture") ?
      obj.dict.at("ceiling_texture") : scene.defaultCeilingTexture;

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      const parser::Object& child = **it;
      string type = child.dict.at("type");

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
  catch (const Exception& ex) {
    delete region;
    EXCEPTION("Error constructing region; " << ex.what());
  }

  return region;
}

//===========================================
// intersectWall
//===========================================
static bool intersectWall(const Region& region, const Circle& circle) {
  for (auto it = region.edges.begin(); it != region.edges.end(); ++it) {
    if (lineSegmentCircleIntersect(circle, (*it)->lseg)) {
      return true;
    }
  }
  return false;
}

//===========================================
// Scene::Scene
//===========================================
Scene::Scene(const string& mapFilePath) {
  defaultFloorHeight = 0;
  defaultCeilingHeight = 100;
  defaultFloorTexture = "cracked_mud";
  defaultCeilingTexture = "grey_stone";

  list<parser::pObject_t> objects;
  parser::parse(mapFilePath, objects);

  for (auto it = objects.begin(); it != objects.end(); ++it) {
    addObject(**it);
  }

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
  textures["crate"] = Texture{QImage("data/crate.png"), Size(30, 30)};
  textures["grey_stone"] = Texture{QImage("data/grey_stone.png"), Size(100, 100)};
  textures["ammo"] = Texture{QImage("data/ammo.png"), Size(100, 100)};
  textures["bad_guy"] = Texture{QImage("data/bad_guy.png"), Size(100, 100)};
}

//===========================================
// Scene::rotateCamera
//===========================================
void Scene::rotateCamera(double da) {
  camera->angle += da;
}

//===========================================
// Scene::translateCamera
//===========================================
void Scene::translateCamera(const Vec2f& dir) {
  Camera& cam = *camera;

  Vec2f dv(cos(cam.angle) * dir.x - sin(cam.angle) * dir.y,
    sin(cam.angle) * dir.x + cos(cam.angle) * dir.y);

  double radius = 20.0;

  Circle circle{cam.pos + dv, radius};
  LineSegment ray(cam.pos, cam.pos + dv);

  bool collision = false;
  for (auto it = currentRegion->edges.begin(); it != currentRegion->edges.end(); ++it) {
    const Edge& edge = **it;

    if (lineSegmentCircleIntersect(circle, edge.lseg)) {
      if (edge.kind == EdgeKind::WALL) {
        collision = true;

        Matrix m(-atan(edge.lseg.line().m), Vec2f());
        Vec2f dv_ = m * dv;
        dv_.y = 0;
        dv_ = m.inverse() * dv_;

        Circle circle2{cam.pos + dv_, radius};

        if (!intersectWall(*currentRegion, circle2)) {
          cam.pos = cam.pos + dv_;
          return;
        }
      }
      else if (edge.kind == EdgeKind::JOINING_EDGE) {
        const JoiningEdge& je = dynamic_cast<const JoiningEdge&>(edge);

        bool crossesLine = distanceFromLine(edge.lseg.line(), cam.pos)
          * distanceFromLine(edge.lseg.line(), cam.pos + dv) < 0;

        if (crossesLine) {
          double floorH = currentRegion->floorHeight;
          currentRegion = je.regionA == currentRegion ? je.regionB : je.regionA;

          cam.height += currentRegion->floorHeight - floorH;

          break;
        }
      }
    }
  }

  if (!collision) {
    cam.pos = cam.pos + dv;
  }
}

//===========================================
// Scene::addObject
//===========================================
void Scene::addObject(const parser::Object& obj) {
  if (obj.dict.at("type") == "region") {
    if (rootRegion) {
      EXCEPTION("Root region already exists");
    }

    Matrix m;
    rootRegion.reset(constructRegion_r(*this, obj, m));
  }
}
