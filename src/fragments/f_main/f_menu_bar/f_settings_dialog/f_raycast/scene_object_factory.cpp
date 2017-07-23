#include <string>
#include <cassert>
#include <sstream>
#include <list>
#include <iterator>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_object_factory.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_graph.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/c_door_behaviour.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"
#include "event.hpp"
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
// getValue
//===========================================
static const string& getValue(const map<string, string>& m, const string& key,
  const string& default_) {

  if (m.find(key) != m.end()) {
    return m.at(key);
  }
  else {
    return default_;
  }
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
static void constructWallDecal(Scene& scene, const parser::Object& obj,
  const Matrix& parentTransform, entityId_t parentId, const LineSegment& wall) {

  Point A = parentTransform * obj.transform * obj.path.points[0];
  Point B = parentTransform * obj.transform * obj.path.points[1];

  double a_ = distance(wall.A, A);
  double b_ = distance(wall.A, B);

  double a = smallest(a_, b_);
  double b = largest(a_, b_);
  double w = b - a;

  if (distanceFromLine(wall.line(), A) > SNAP_DISTANCE
    || distanceFromLine(wall.line(), B) > SNAP_DISTANCE) {

    return;
  }
  if (a < 0 || b < 0) {
    return;
  }
  if (a > wall.length() || b > wall.length()) {
    return;
  }

  DBG_PRINT("Constructing WallDecal\n");

  double r = std::stod(getValue(obj.dict, "aspect_ratio"));
  Size size(w, w / r);

  double y = std::stod(getValue(obj.dict, "y"));
  Point pos(a, y);

  string texture = getValue(obj.dict, "texture");

  WallDecal* decal = new WallDecal(Component::getNextId(), parentId);
  decal->texture = texture;
  decal->size = size;
  decal->pos = pos;

  scene.addComponent(pComponent_t(decal));
}

//===========================================
// constructWalls
//===========================================
static void constructWalls(Scene& scene, map<Point, bool>& endpoints, const parser::Object& obj,
  Region& region, const Matrix& parentTransform) {

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

    Wall* wall = new Wall(Component::getNextId(), region.entityId());

    wall->lseg.A = obj.path.points[j];
    wall->lseg.B = obj.path.points[i];
    wall->lseg = transform(wall->lseg, m);

    wall->region = &region;
    wall->texture = getValue(obj.dict, "texture");

    walls.push_back(wall);

    scene.addComponent(pComponent_t(wall));

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      if (getValue((*it)->dict, "type") == "wall_decal") {
        constructWallDecal(scene, **it, m, wall->entityId(), wall->lseg);
      }
    }
  }

  snapEndpoint(endpoints, walls.front()->lseg.A);
  snapEndpoint(endpoints, walls.back()->lseg.B);
}

//===========================================
// constructFloorDecal
//===========================================
static void constructFloorDecal(Scene& scene, const parser::Object& obj,
  const Matrix& parentTransform, entityId_t parentId) {

  DBG_PRINT("Constructing FloorDecal\n");

  string texture = getValue(obj.dict, "texture");

  Point pos = obj.path.points[0];
  Size size = obj.path.points[2] - obj.path.points[0];

  assert(size.x > 0);
  assert(size.y > 0);

  Matrix m(0, pos);

  FloorDecal* decal = new FloorDecal(Component::getNextId(), parentId);
  decal->texture = texture;
  decal->size = size;
  decal->transform = parentTransform * obj.transform * m;

  scene.addComponent(pComponent_t(decal));
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
static void constructSprite(Scene& scene, const parser::Object& obj, Region& region,
  const Matrix& parentTransform) {

  DBG_PRINT("Constructing Sprite\n");

  if (getValue(obj.dict, "subtype") == "bad_guy") {
    BadGuy* sprite = new BadGuy(Component::getNextId(), region.entityId());
    Matrix m = transformFromTriangle(obj.path);
    sprite->setTransform(parentTransform * obj.transform * m);
    sprite->region = &region;

    scene.addComponent(pComponent_t(sprite));
  }
  else if (getValue(obj.dict, "subtype") == "ammo") {
    Ammo* sprite = new Ammo(Component::getNextId(), region.entityId());
    Matrix m = transformFromTriangle(obj.path);
    sprite->setTransform(parentTransform * obj.transform * m);
    sprite->region = &region;

    scene.addComponent(pComponent_t(sprite));
  }
  else {
    EXCEPTION("Error constructing sprite of unknown type");
  }
}

//===========================================
// constructJoiningEdges
//===========================================
static void constructJoiningEdges(Scene& scene, map<Point, bool>& endpoints,
  const parser::Object& obj, Region* region, const Matrix& parentTransform) {

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

    JoiningEdge* je = new JoiningEdge(Component::getNextId(), region->entityId());

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

    scene.addComponent(pComponent_t(je));
  }

  snapEndpoint(endpoints, joiningEdges.front()->lseg.A);
  snapEndpoint(endpoints, joiningEdges.back()->lseg.B);
}

//===========================================
// constructDoor
//===========================================
static void constructDoor(BehaviourSystem& behaviourSystem, const parser::Object& obj,
  Region& region) {

  CDoorBehaviour* behaviour = new CDoorBehaviour(region.entityId(), region);
  behaviourSystem.addComponent(pComponent_t(behaviour));
}

//===========================================
// constructRegion_r
//===========================================
static void constructRegion_r(Scene& scene, SceneGraph& sg, BehaviourSystem& behaviourSystem,
  const parser::Object& obj, Region* parent, const Matrix& parentTransform, map<Point,
  bool>& endpoints) {

  DBG_PRINT("Constructing Region\n");

  entityId_t entityId = Component::getNextId();
  entityId_t parentId = parent == nullptr ? -1 : parent->entityId();

  Region* region = new Region(entityId, parentId);
  region->parent = parent;

  try {
    scene.addComponent(pComponent_t(region));

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
        constructRegion_r(scene, sg, behaviourSystem, child, region, transform, endpoints);
      }
      else if (type == "wall") {
        constructWalls(scene, endpoints, child, *region, transform);
      }
      else if (type == "joining_edge") {
        constructJoiningEdges(scene, endpoints, child, region, transform);
      }
      else if (type == "sprite") {
        constructSprite(scene, child, *region, transform);
      }
      else if (type == "floor_decal") {
        constructFloorDecal(scene, child, transform, entityId);
      }
      else if (type == "player") {
        if (sg.player) {
          EXCEPTION("Player already exists");
        }
        sg.player.reset(constructPlayer(child, *region, transform, sg.viewport));
        sg.currentRegion = region;
      }
    }

    if (getValue(obj.dict, "subtype", "") == "door") {
      constructDoor(behaviourSystem, obj, *region);
    }
  }
  catch (Exception& ex) {
    //delete region;
    ex.prepend("Error constructing region; ");
    throw ex;
  }
  catch (const std::exception& ex) {
    //delete region;
    EXCEPTION("Error constructing region; " << ex.what());
  }
}

//===========================================
// constructRootRegion
//===========================================
void constructRootRegion(Scene& scene, BehaviourSystem& behaviourSystem,
  const parser::Object& obj) {

  SceneGraph& sg = scene.sg;

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

  constructRegion_r(scene, sg, behaviourSystem, obj, nullptr, m, endpoints);

  for (auto it = endpoints.begin(); it != endpoints.end(); ++it) {
    if (it->second == false) {
      EXCEPTION("There are unconnected endpoints");
    }
  }

  if (!sg.player) {
    EXCEPTION("Scene must contain the player");
  }

  scene.connectRegions();

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
