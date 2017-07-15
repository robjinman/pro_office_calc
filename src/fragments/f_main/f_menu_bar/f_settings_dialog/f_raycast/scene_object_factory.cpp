#include <string>
#include <cassert>
#include <sstream>
#include <list>
#include <iterator>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_object_factory.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_graph.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::stringstream;
using std::unique_ptr;
using std::function;
using std::string;
using std::list;
using std::map;


const double SNAP_DISTANCE = 4.0;


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
static void connectSubregions_r(SceneGraph& sg, Region& region) {
  if (region.children.size() == 0) {
    return;
  }

  for (auto it = region.children.begin(); it != region.children.end(); ++it) {
    Region& r = **it;
    connectSubregions_r(sg, r);

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

                    sg.edges.push_back(pEdge_t(combined));
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
// snapEndpoint
//===========================================
static void snapEndpoint(map<Point, bool>& endpoints, Point& pt) {
  for (auto it = endpoints.begin(); it != endpoints.end(); ++it) {
    if (distance(pt, it->first) <= SNAP_DISTANCE) {
      pt = it->first;
      it->second = true;
      return;
    }
  }

  endpoints[pt] = false;
};

//===========================================
// constructWallDecal
//
// wall is the wall's transformed line segment
//===========================================
static WallDecal* constructWallDecal(const parser::Object& obj, const Matrix& parentTransform,
  const LineSegment& wall) {

  Point A = parentTransform * obj.transform * obj.path.points[0];
  Point B = parentTransform * obj.transform * obj.path.points[1];

  double a_ = distance(wall.A, A);
  double b_ = distance(wall.A, B);

  double a = smallest(a_, b_);
  double b = largest(a_, b_);
  double w = b - a;

  if (distanceFromLine(wall.line(), A) > SNAP_DISTANCE
    || distanceFromLine(wall.line(), B) > SNAP_DISTANCE) {

    return nullptr;
  }
  if (a < 0 || b < 0) {
    return nullptr;
  }
  if (a > wall.length() || b > wall.length()) {
    return nullptr;
  }

  DBG_PRINT("Constructing WallDecal\n");

  double r = std::stod(getValue(obj.dict, "aspect_ratio"));
  Size size(w, w / r);

  double y = std::stod(getValue(obj.dict, "y"));
  Point pos(a, y);

  string texture = getValue(obj.dict, "texture");

  return new WallDecal{
    texture,
    size,
    pos
  };
}

//===========================================
// constructWalls
//===========================================
static list<Wall*> constructWalls(const parser::Object& obj, Region* region,
  const Matrix& parentTransform) {

  DBG_PRINT("Constructing Walls\n");

  Matrix m = parentTransform * obj.transform;

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
    wall->lseg = transform(wall->lseg, m);

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      if (getValue((*it)->dict, "type") == "wall_decal") {
        WallDecal* decal = constructWallDecal(**it, m, wall->lseg);
        if (decal != nullptr) {
          wall->decals.push_back(pWallDecal_t(decal));
        }
      }
    }

    wall->region = region;
    wall->texture = getValue(obj.dict, "texture");

    walls.push_back(wall);
  }

  return walls;
}

//===========================================
// constructFloorDecal
//===========================================
static FloorDecal* constructFloorDecal(const parser::Object& obj, const Matrix& parentTransform) {
  DBG_PRINT("Constructing FloorDecal\n");

  string texture = getValue(obj.dict, "texture");

  Point pos = obj.path.points[0];
  Size size = obj.path.points[2] - obj.path.points[0];

  assert(size.x > 0);
  assert(size.y > 0);

  Matrix m(0, pos);

  return new FloorDecal{
    texture,
    size,
    parentTransform * obj.transform * m
  };
}

//===========================================
// constructPlayer
//===========================================
static Player* constructPlayer(const parser::Object& obj, const Region& region,
  const Matrix& parentTransform, const Size& viewport) {

  DBG_PRINT("Constructing Player\n");

  double tallness = std::stod(getValue(obj.dict, "tallness"));

  Camera* camera = new Camera(viewport.x, DEG_TO_RAD(60), DEG_TO_RAD(50));
  camera->setTransform(parentTransform * obj.transform * transformFromTriangle(obj.path));
  camera->height = tallness + region.floorHeight;

  Player* player = new Player(tallness, unique_ptr<Camera>(camera));
  return player;
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
// constructRegion_r
//===========================================
static Region* constructRegion_r(SceneGraph& sg, const parser::Object& obj,
  const Matrix& parentTransform, map<Point, bool>& endpoints) {

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
      std::stod(getValue(obj.dict, "floor_height")) : sg.defaults.floorHeight;

    region->ceilingHeight = contains<string>(obj.dict, "ceiling_height") ?
      std::stod(getValue(obj.dict, "ceiling_height")) : sg.defaults.ceilingHeight;

    region->floorTexture = contains<string>(obj.dict, "floor_texture") ?
      getValue(obj.dict, "floor_texture") : sg.defaults.floorTexture;

    region->ceilingTexture = contains<string>(obj.dict, "ceiling_texture") ?
      getValue(obj.dict, "ceiling_texture") : sg.defaults.ceilingTexture;

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      const parser::Object& child = **it;
      string type = getValue(child.dict, "type");

      if (type == "region") {
        region->children.push_back(pRegion_t(constructRegion_r(sg, child, transform,
          endpoints)));
      }
      else if (type == "wall") {
        list<Wall*> walls = constructWalls(child, region, transform);
        for (auto jt = walls.begin(); jt != walls.end(); ++jt) {
          sg.edges.push_back(pEdge_t(*jt));
          region->edges.push_back(*jt);
        }
        snapEndpoint(endpoints, walls.front()->lseg.A);
        snapEndpoint(endpoints, walls.back()->lseg.B);
      }
      else if (type == "joining_edge") {
        list<JoiningEdge*> joiningEdges = constructJoiningEdges(child, region, transform);
        for (auto jt = joiningEdges.begin(); jt != joiningEdges.end(); ++jt) {
          region->edges.push_back(*jt);
        }
        snapEndpoint(endpoints, joiningEdges.front()->lseg.A);
        snapEndpoint(endpoints, joiningEdges.back()->lseg.B);
      }
      else if (type == "sprite") {
        region->sprites.push_back(pSprite_t(constructSprite(child, *region, transform)));
      }
      else if (type == "floor_decal") {
        region->floorDecals.push_back(pFloorDecal_t(constructFloorDecal(child, transform)));
      }
      else if (type == "player") {
        if (sg.player) {
          EXCEPTION("Player already exists");
        }
        sg.player.reset(constructPlayer(child, *region, transform, sg.viewport));
        sg.currentRegion = region;
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
// constructRootRegion
//===========================================
void constructRootRegion(SceneGraph& sg, const parser::Object& obj) {
  if (getValue(obj.dict, "type") != "region") {
    EXCEPTION("Expected object of type 'region'");
  }

  if (sg.rootRegion) {
    EXCEPTION("Root region already exists");
  }

  sg.viewport.x = 10.0 * 320.0 / 240.0; // TODO: Read from map file
  sg.viewport.y = 10.0;

  map<Point, bool> endpoints;
  Matrix m;
  sg.rootRegion.reset(constructRegion_r(sg, obj, m, endpoints));

  for (auto it = endpoints.begin(); it != endpoints.end(); ++it) {
    if (it->second == false) {
      EXCEPTION("There are unconnected endpoints");
    }
  }

  if (!sg.player) {
    EXCEPTION("Scene must contain the player");
  }

  connectSubregions_r(sg, *sg.rootRegion);

  sg.textures["default"] = Texture{QImage("data/default.png"), Size(100, 100)};
  sg.textures["light_bricks"] = Texture{QImage("data/light_bricks.png"), Size(100, 100)};
  sg.textures["dark_bricks"] = Texture{QImage("data/dark_bricks.png"), Size(100, 100)};
  sg.textures["door"] = Texture{QImage("data/door.png"), Size(100, 100)};
  sg.textures["cracked_mud"] = Texture{QImage("data/cracked_mud.png"), Size(100, 100)};
  sg.textures["dirt"] = Texture{QImage("data/dirt.png"), Size(100, 100)};
  sg.textures["crate"] = Texture{QImage("data/crate.png"), Size(30, 30)};
  sg.textures["grey_stone"] = Texture{QImage("data/grey_stone.png"), Size(100, 100)};
  sg.textures["stone_slabs"] = Texture{QImage("data/stone_slabs.png"), Size(100, 100)};
  sg.textures["ammo"] = Texture{QImage("data/ammo.png"), Size(100, 100)};
  sg.textures["bad_guy"] = Texture{QImage("data/bad_guy.png"), Size(100, 100)};
  sg.textures["sky"] = Texture{QImage("data/sky.png"), Size()};
  sg.textures["beer"] = Texture{QImage("data/beer.png"), Size()};
}
