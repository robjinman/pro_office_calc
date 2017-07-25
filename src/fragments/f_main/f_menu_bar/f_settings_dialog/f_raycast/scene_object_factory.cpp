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
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_system.hpp"
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
static void constructWallDecal(EntityManager& em, const parser::Object& obj,
  const Matrix& parentTransform, entityId_t parentId, const LineSegment& wall) {

  SpatialSystem& spatialSystem = em.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = em.system<RenderSystem&>(ComponentKind::C_RENDER);

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

  entityId_t id = Component::getNextId();

  VRect* vRect = new VRect(id, parentId, size);
  vRect->pos = pos;

  spatialSystem.addComponent(pComponent_t(vRect));

  CWallDecal* decal = new CWallDecal(id, parentId);
  decal->texture = texture;

  renderSystem.addComponent(pComponent_t(decal));
}

//===========================================
// constructWalls
//===========================================
static void constructWalls(EntityManager& em, map<Point, bool>& endpoints,
  const parser::Object& obj, CZone& zone, CRegion& region, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = em.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = em.system<RenderSystem>(ComponentKind::C_RENDER);

  DBG_PRINT("Constructing Walls\n");

  Matrix m = parentTransform * obj.transform;

  list<CHardEdge*> edges;

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

    entityId_t id = Component::getNextId();

    CHardEdge* edge = new CHardEdge(id, zone.entityId());

    edge->lseg.A = obj.path.points[j];
    edge->lseg.B = obj.path.points[i];
    edge->lseg = transform(edge->lseg, m);
    edge->zone = &zone;

    edges.push_back(edge);

    spatialSystem.addComponent(pComponent_t(edge));

    CWall* wall = new CWall(id, region.entityId());

    wall->region = &region;
    wall->texture = getValue(obj.dict, "texture");

    renderSystem.addComponent(pComponent_t(wall));

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      if (getValue((*it)->dict, "type") == "wall_decal") {
        constructWallDecal(em, **it, m, id, edge->lseg);
      }
    }
  }

  snapEndpoint(endpoints, edges.front()->lseg.A);
  snapEndpoint(endpoints, edges.back()->lseg.B);
}

//===========================================
// constructFloorDecal
//===========================================
static void constructFloorDecal(EntityManager& em, const parser::Object& obj,
  const Matrix& parentTransform, entityId_t parentId) {

  SpatialSystem& spatialSystem = em.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = em.system<RenderSystem>(ComponentKind::C_RENDER);

  DBG_PRINT("Constructing FloorDecal\n");

  string texture = getValue(obj.dict, "texture");

  Point pos = obj.path.points[0];
  Size size = obj.path.points[2] - obj.path.points[0];

  assert(size.x > 0);
  assert(size.y > 0);

  Matrix m(0, pos);

  entityId_t id = Component::getNextId();

  CHRect* hRect = new CHRect(id, parentId);
  hRect->size = size;
  hRect->transform = parentTransform * obj.transform * m;

  spatialSystem.addComponent(pComponent_t(hRect));

  CFloorDecal* decal = new CFloorDecal(id, parentId);
  decal->texture = texture;

  renderSystem.addComponent(pComponent_t(decal));
}

//===========================================
// constructPlayer
//===========================================
static Player* constructPlayer(const parser::Object& obj, const CZone& zone,
  const Matrix& parentTransform, const Size& viewport) {

  DBG_PRINT("Constructing Player\n");

  double tallness = std::stod(getValue(obj.dict, "tallness"));

  Camera* camera = new Camera(viewport.x, DEG_TO_RAD(60), DEG_TO_RAD(50));
  camera->setTransform(parentTransform * obj.transform * transformFromTriangle(obj.path));
  camera->height = tallness + zone.floorHeight;

  Player* player = new Player(tallness, unique_ptr<Camera>(camera));
  return player;
}

//===========================================
// constructSprite
//===========================================
static void constructSprite(EntityManager& em, const parser::Object& obj, CZone& zone,
  CRegion& region, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = em.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = em.system<RenderSystem>(ComponentKind::C_RENDER);

  DBG_PRINT("Constructing Sprite\n");

  if (getValue(obj.dict, "subtype") == "bad_guy") {
    entityId_t id = Component::getNextId();

    VRect* vRect = new VRect(id, zone.entityId(), Size(70, 70));
    Matrix m = transformFromTriangle(obj.path);
    vRect->setTransform(parentTransform * obj.transform * m);
    vRect->zone = &zone;

    spatialSystem.addComponent(pComponent_t(vRect));

    CBadGuy* sprite = new CBadGuy(id, zone.entityId());
    sprite->region = &region;

    renderSystem.addComponent(pComponent_t(sprite));
  }
  else if (getValue(obj.dict, "subtype") == "ammo") {
    entityId_t id = Component::getNextId();

    VRect* vRect = new VRect(id, zone.entityId(), Size(30, 15));
    Matrix m = transformFromTriangle(obj.path);
    vRect->setTransform(parentTransform * obj.transform * m);
    vRect->zone = &zone;

    spatialSystem.addComponent(pComponent_t(vRect));

    CAmmo* sprite = new CAmmo(id, zone.entityId());
    sprite->region = &region;

    renderSystem.addComponent(pComponent_t(sprite));
  }
  else {
    EXCEPTION("Error constructing sprite of unknown type");
  }
}

//===========================================
// constructCSoftEdges
//===========================================
static void constructCSoftEdges(EntityManager& em, map<Point, bool>& endpoints,
  const parser::Object& obj, entityId_t parentId, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = em.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = em.system<RenderSystem>(ComponentKind::C_RENDER);

  DBG_PRINT("Constructing CSoftEdges\n");

  list<CSoftEdge*> edges;

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

    entityId_t entityId = Component::getNextId();

    CSoftEdge* edge = new CSoftEdge(entityId, parentId, Component::getNextId());

    edge->lseg.A = obj.path.points[j];
    edge->lseg.B = obj.path.points[i];
    edge->lseg = transform(edge->lseg, parentTransform * obj.transform);

    edges.push_back(edge);

    spatialSystem.addComponent(pComponent_t(edge));

    CJoin* boundary = new CJoin(entityId, parentId, Component::getNextId());

    if (contains<string>(obj.dict, "top_texture")) {
      boundary->topTexture = getValue(obj.dict, "top_texture");
    }
    if (contains<string>(obj.dict, "bottom_texture")) {
      boundary->bottomTexture = getValue(obj.dict, "bottom_texture");
    }

    renderSystem.addComponent(pComponent_t(boundary));
  }

  snapEndpoint(endpoints, edges.front()->lseg.A);
  snapEndpoint(endpoints, edges.back()->lseg.B);
}

//===========================================
// constructDoor
//===========================================
static void constructDoor(EntityManager& em, const parser::Object& obj, CZone& zone) {
  BehaviourSystem& behaviourSystem = em.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

  CDoorBehaviour* behaviour = new CDoorBehaviour(zone.entityId(), zone);
  behaviourSystem.addComponent(pComponent_t(behaviour));
}

//===========================================
// constructRegion_r
//===========================================
static void constructRegion_r(EntityManager& em, const parser::Object& obj, CZone* parentZone,
  CRegion* parentRegion, const Matrix& parentTransform, map<Point, bool>& endpoints) {

  RenderSystem& renderSystem = em.system<RenderSystem>(ComponentKind::C_RENDER);
  SpatialSystem& spatialSystem = em.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  SceneGraph& sg = spatialSystem.sg;
  RenderGraph& rg = renderSystem.rg;

  DBG_PRINT("Constructing Region\n");

  entityId_t entityId = Component::getNextId();
  entityId_t parentId = parentZone == nullptr ? -1 : parentZone->entityId();

  CZone* zone = new CZone(entityId, parentId);
  zone->parent = parentZone;

  CRegion* region = new CRegion(entityId, parentId);

  try {
    spatialSystem.addComponent(pComponent_t(zone));
    renderSystem.addComponent(pComponent_t(region));

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

    zone->floorHeight = contains<string>(obj.dict, "floor_height") ?
      std::stod(getValue(obj.dict, "floor_height")) : sg.defaults.floorHeight;

    zone->ceilingHeight = contains<string>(obj.dict, "ceiling_height") ?
      std::stod(getValue(obj.dict, "ceiling_height")) : sg.defaults.ceilingHeight;

    region->floorTexture = contains<string>(obj.dict, "floor_texture") ?
      getValue(obj.dict, "floor_texture") : rg.defaults.floorTexture;

    region->ceilingTexture = contains<string>(obj.dict, "ceiling_texture") ?
      getValue(obj.dict, "ceiling_texture") : rg.defaults.ceilingTexture;

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      const parser::Object& child = **it;
      string type = getValue(child.dict, "type");

      if (type == "region") {
        constructRegion_r(em, child, zone, region, transform, endpoints);
      }
      else if (type == "wall") {
        constructWalls(em, endpoints, child, *zone, *region, transform);
      }
      else if (type == "joining_edge") {
        constructCSoftEdges(em, endpoints, child, entityId, transform);
      }
      else if (type == "sprite") {
        constructSprite(em, child, *zone, *region, transform);
      }
      else if (type == "floor_decal") {
        constructFloorDecal(em, child, transform, entityId);
      }
      else if (type == "player") {
        if (sg.player) {
          EXCEPTION("Player already exists");
        }
        sg.player.reset(constructPlayer(child, *zone, transform, rg.viewport));
        sg.player->currentRegion = entityId;
      }
    }

    if (getValue(obj.dict, "subtype", "") == "door") {
      constructDoor(em, obj, *zone);
    }
  }
  catch (Exception& ex) {
    //delete zone;
    ex.prepend("Error constructing region; ");
    throw ex;
  }
  catch (const std::exception& ex) {
    //delete zone;
    EXCEPTION("Error constructing region; " << ex.what());
  }
}

//===========================================
// constructRootRegion
//===========================================
void constructRootRegion(EntityManager& em, const parser::Object& obj) {
  RenderSystem& renderSystem = em.system<RenderSystem>(ComponentKind::C_RENDER);
  SpatialSystem& spatialSystem = em.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  RenderGraph& rg = renderSystem.rg;
  SceneGraph& sg = spatialSystem.sg;

  if (getValue(obj.dict, "type") != "region") {
    EXCEPTION("Expected object of type 'region'");
  }

  if (sg.rootZone || rg.rootRegion) {
    EXCEPTION("Root region already exists");
  }

  rg.viewport.x = 10.0 * 320.0 / 240.0; // TODO: Read from map file
  rg.viewport.y = 10.0;

  map<Point, bool> endpoints;
  Matrix m;

  constructRegion_r(em, obj, nullptr, nullptr, m, endpoints);

  for (auto it = endpoints.begin(); it != endpoints.end(); ++it) {
    if (it->second == false) {
      EXCEPTION("There are unconnected endpoints");
    }
  }

  if (!sg.player) {
    EXCEPTION("SpatialSystem must contain the player");
  }

  spatialSystem.connectZones();
  renderSystem.connectRegions();

  rg.textures["default"] = Texture{QImage("data/default.png"), Size(100, 100)};
  rg.textures["light_bricks"] = Texture{QImage("data/light_bricks.png"), Size(100, 100)};
  rg.textures["dark_bricks"] = Texture{QImage("data/dark_bricks.png"), Size(100, 100)};
  rg.textures["door"] = Texture{QImage("data/door.png"), Size(100, 100)};
  rg.textures["cracked_mud"] = Texture{QImage("data/cracked_mud.png"), Size(100, 100)};
  rg.textures["dirt"] = Texture{QImage("data/dirt.png"), Size(100, 100)};
  rg.textures["crate"] = Texture{QImage("data/crate.png"), Size(30, 30)};
  rg.textures["grey_stone"] = Texture{QImage("data/grey_stone.png"), Size(100, 100)};
  rg.textures["stone_slabs"] = Texture{QImage("data/stone_slabs.png"), Size(100, 100)};
  rg.textures["ammo"] = Texture{QImage("data/ammo.png"), Size(100, 100)};
  rg.textures["bad_guy"] = Texture{QImage("data/bad_guy.png"), Size(100, 100)};
  rg.textures["sky"] = Texture{QImage("data/sky.png"), Size()};
  rg.textures["beer"] = Texture{QImage("data/beer.png"), Size()};
}
