#include <list>
#include <cassert>
#include "raycast/geometry_factory.hpp"
#include "raycast/root_factory.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/time_service.hpp"
#include "utils.hpp"


using std::stringstream;
using std::unique_ptr;
using std::function;
using std::string;
using std::list;
using std::map;
using std::set;
using std::vector;


const double SNAP_DISTANCE = 1.0;


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
// GeometryFactory::constructPath
//===========================================
bool GeometryFactory::constructPath(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  Matrix m = parentTransform * obj.groupTransform * obj.pathTransform;

  CPath* path = new CPath(makeIdForObj(obj), parentId);

  for (unsigned int i = 0; i < obj.path.points.size(); ++i) {
    path->points.push_back(m * obj.path.points[i]);
  }

  spatialSystem.addComponent(pComponent_t(path));

  return true;
}

//===========================================
// GeometryFactory::constructWallDecal
//===========================================
bool GeometryFactory::constructWallDecal(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem&>(ComponentKind::C_RENDER);

  const CEdge& edge = dynamic_cast<const CEdge&>(spatialSystem.getComponent(parentId));
  const LineSegment& wall = edge.lseg;

  LineSegment lseg(obj.path.points[0], obj.path.points[1]);
  lseg = transform(lseg, parentTransform * obj.groupTransform * obj.pathTransform);

  Point A = lseg.A;
  Point B = lseg.B;

  double a_ = distance(wall.A, A);
  double b_ = distance(wall.A, B);

  double a = smallest(a_, b_);
  double b = largest(a_, b_);
  double w = b - a;

  if (distanceFromLine(wall.line(), A) > SNAP_DISTANCE
    || distanceFromLine(wall.line(), B) > SNAP_DISTANCE) {

    return false;
  }

  double delta = SNAP_DISTANCE;
  if (!(isBetween(A.x, wall.A.x, wall.B.x, delta)
    && isBetween(A.y, wall.A.y, wall.B.y, delta)
    && isBetween(B.x, wall.A.x, wall.B.x, delta)
    && isBetween(B.y, wall.A.y, wall.B.y, delta))) {

    return false;
  }

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(edge.parentId));

  double r = std::stod(getValue(obj.dict, "aspect_ratio"));
  Size size(w, w / r);

  double y = std::stod(getValue(obj.dict, "y"));
  Point pos(a, y);

  int zIndex = std::stoi(getValue(obj.dict, "z_index", "1"));

  string texture = getValue(obj.dict, "texture", "default");

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  CVRect* vRect = new CVRect(entityId, parentId, size);
  vRect->pos = pos;
  vRect->zone = &zone;

  spatialSystem.addComponent(pComponent_t(vRect));

  CWallDecal* decal = new CWallDecal(entityId, parentId);
  decal->texture = texture;
  decal->zIndex = zIndex;

  renderSystem.addComponent(pComponent_t(decal));

  return true;
}

//===========================================
// GeometryFactory::constructWalls
//===========================================
bool GeometryFactory::constructWalls(parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(parentId));
  CRegion& region = dynamic_cast<CRegion&>(renderSystem.getComponent(parentId));

  Matrix m = parentTransform * obj.groupTransform * obj.pathTransform;

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

    entityId_t entityId = Component::getNextId();

    CHardEdge* edge = new CHardEdge(entityId, zone.entityId());

    edge->lseg.A = obj.path.points[j];
    edge->lseg.B = obj.path.points[i];
    edge->lseg = transform(edge->lseg, m);
    edge->zone = &zone;

    edges.push_back(edge);

    snapEndpoint(m_endpoints, edge->lseg.A);
    snapEndpoint(m_endpoints, edge->lseg.B);

    spatialSystem.addComponent(pComponent_t(edge));

    CWall* wall = new CWall(entityId, region.entityId());

    wall->region = &region;
    wall->texture = getValue(obj.dict, "texture", "default");

    renderSystem.addComponent(pComponent_t(wall));

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      m_rootFactory.constructObject((*it)->type, -1, **it, entityId,
        parentTransform * obj.groupTransform);
    }
  }

  if (edges.size() == 0) {
    return false;
  }

  return true;
}

//===========================================
// GeometryFactory::constructFloorDecal
//===========================================
bool GeometryFactory::constructFloorDecal(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  string texName = getValue(obj.dict, "texture", "default");

  if (obj.path.points.size() != 4) {
    EXCEPTION("Floor decal path must have exactly 4 points");
  }

  Point pos = obj.path.points[0];
  Size size = obj.path.points[2] - obj.path.points[0];

  assert(size.x > 0);
  assert(size.y > 0);

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  Matrix m = parentTransform * obj.groupTransform * obj.pathTransform * Matrix(0, pos);

  CHRect* hRect = new CHRect(entityId, parentId);
  hRect->size = size;
  hRect->transform = m.inverse();

  spatialSystem.addComponent(pComponent_t(hRect));

  CFloorDecal* decal = new CFloorDecal(entityId, parentId);
  decal->texture = texName;

  renderSystem.addComponent(pComponent_t(decal));

  return true;
}

//===========================================
// GeometryFactory::constructPortal
//===========================================
bool GeometryFactory::constructPortal(parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  if (obj.path.points.size() != 2) {
    EXCEPTION("Portal must contain 1 line segment");
  }

  entityId_t entityId = Component::getNextId();
  entityId_t joinId = Component::getIdFromString(getValue(obj.dict, "pair_name"));

  CSoftEdge* edge = new CSoftEdge(entityId, parentId, joinId);

  edge->lseg.A = obj.path.points[0];
  edge->lseg.B = obj.path.points[1];
  edge->lseg = transform(edge->lseg, parentTransform * obj.groupTransform * obj.pathTransform);
  edge->isPortal = true;

  snapEndpoint(m_endpoints, edge->lseg.A);
  snapEndpoint(m_endpoints, edge->lseg.B);

  spatialSystem.addComponent(pComponent_t(edge));

  CJoin* boundary = new CJoin(entityId, parentId, Component::getNextId());
  boundary->topTexture = getValue(obj.dict, "top_texture", "default");
  boundary->bottomTexture = getValue(obj.dict, "bottom_texture", "default");

  renderSystem.addComponent(pComponent_t(boundary));

  return true;
}

//===========================================
// GeometryFactory::constructBoundaries
//===========================================
bool GeometryFactory::constructBoundaries(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  list<CSoftEdge*> edges;

  if (obj.path.points.size() > 2 && entityId != -1) {
    EXCEPTION("Cannot specify entityId for multiple joins");
  }

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

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

    CSoftEdge* edge = new CSoftEdge(entityId, parentId, Component::getNextId());

    edge->lseg.A = obj.path.points[j];
    edge->lseg.B = obj.path.points[i];
    edge->lseg = transform(edge->lseg, parentTransform * obj.groupTransform * obj.pathTransform);

    snapEndpoint(m_endpoints, edge->lseg.A);
    snapEndpoint(m_endpoints, edge->lseg.B);

    edges.push_back(edge);

    spatialSystem.addComponent(pComponent_t(edge));

    CJoin* boundary = new CJoin(entityId, parentId, Component::getNextId());
    boundary->topTexture = getValue(obj.dict, "top_texture", "default");
    boundary->bottomTexture = getValue(obj.dict, "bottom_texture", "default");

    renderSystem.addComponent(pComponent_t(boundary));

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      m_rootFactory.constructObject((*it)->type, -1, **it, entityId,
        parentTransform * obj.groupTransform);
    }

    entityId = Component::getNextId();
  }

  return true;
}

//===========================================
// GeometryFactory::constructRegion_r
//===========================================
bool GeometryFactory::constructRegion_r(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderGraph& rg = renderSystem.rg;

  CZone* parentZone = parentId == -1 ? nullptr
    : dynamic_cast<CZone*>(&spatialSystem.getComponent(parentId));

  CRegion* parentRegion = parentId == -1 ? nullptr
    : dynamic_cast<CRegion*>(&renderSystem.getComponent(parentId));

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  CZone* zone = new CZone(entityId, parentId);
  zone->parent = parentZone;

  CRegion* region = new CRegion(entityId, parentId);

  try {
    spatialSystem.addComponent(pComponent_t(zone));
    renderSystem.addComponent(pComponent_t(region));

    if (obj.path.points.size() > 0) {
      EXCEPTION("Region has unexpected path");
    }

    if (contains<string>(obj.dict, "has_ceiling")) {
      string s = getValue(obj.dict, "has_ceiling");
      if (s == "true") {
        region->hasCeiling = true;
        zone->hasCeiling = true;
      }
      else if (s == "false") {
        region->hasCeiling = false;
        zone->hasCeiling = false;
      }
      else {
        EXCEPTION("has_ceiling must be either 'true' or 'false'");
      }
    }
    else {
      if (parentZone != nullptr) {
        region->hasCeiling = parentZone->hasCeiling;
        zone->hasCeiling = parentZone->hasCeiling;
      }
    }

    double parentFloorHeight = parentZone ? parentZone->floorHeight : 0;
    double parentCeilingHeight = parentZone ? parentZone->ceilingHeight : 0;

    if (contains<string>(obj.dict, "floor_height")) {
      if (contains<string>(obj.dict, "floor_offset")) {
        EXCEPTION("Region cannot specify both floor_height and floor_offset");
      }

      zone->floorHeight = std::stod(getValue(obj.dict, "floor_height"));
    }
    else if (contains<string>(obj.dict, "floor_offset")) {
      if (contains<string>(obj.dict, "floor_height")) {
        EXCEPTION("Region cannot specify both floor_height and floor_offset");
      }

      zone->floorHeight = parentFloorHeight + std::stod(getValue(obj.dict, "floor_offset"));
    }
    else {
      if (parentZone != nullptr) {
        zone->floorHeight = parentZone->floorHeight;
      }
    }

    if (contains<string>(obj.dict, "ceiling_height")) {
      if (contains<string>(obj.dict, "ceiling_offset")) {
        EXCEPTION("Region cannot specify both ceiling_height and ceiling_offset");
      }

      // Ceiling height is relative to floor height
      zone->ceilingHeight = zone->floorHeight + std::stod(getValue(obj.dict, "ceiling_height"));
    }
    else if (contains<string>(obj.dict, "ceiling_offset")) {
      if (contains<string>(obj.dict, "ceiling_height")) {
        EXCEPTION("Region cannot specify both ceiling_height and ceiling_offset");
      }

      zone->ceilingHeight = std::stod(getValue(obj.dict, "ceiling_offset")) + parentCeilingHeight;
    }
    else {
      if (parentZone != nullptr) {
        zone->ceilingHeight = parentZone->ceilingHeight;
      }
    }

    string defaultFloorTex = parentRegion ?
      parentRegion->floorTexture :
      rg.defaults.floorTexture;

    string defaultCeilingTex = parentRegion ?
      parentRegion->ceilingTexture :
      rg.defaults.ceilingTexture;

    region->floorTexture = getValue(obj.dict, "floor_texture", defaultFloorTex);
    region->ceilingTexture =  getValue(obj.dict, "ceiling_texture", defaultCeilingTex);

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      parser::Object& child = **it;

      m_rootFactory.constructObject(child.type, -1, child, entityId,
        parentTransform * obj.groupTransform);
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

  return true;
}

//===========================================
// GeometryFactory::constructRootRegion
//===========================================
bool GeometryFactory::constructRootRegion(parser::Object& obj) {
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  RenderGraph& rg = renderSystem.rg;
  SceneGraph& sg = spatialSystem.sg;

  if (obj.type != "region") {
    EXCEPTION("Expected object of type 'region'");
  }

  if (sg.rootZone || rg.rootRegion) {
    EXCEPTION("Root region already exists");
  }

  rg.viewport.x = 10.0 * 320.0 / 240.0; // TODO: Read from map file
  rg.viewport.y = 10.0;

  m_endpoints.clear();
  Matrix m;

  if (constructRegion_r(-1, obj, -1, m)) {
    for (auto it = m_endpoints.begin(); it != m_endpoints.end(); ++it) {
      if (it->second == false) {
        EXCEPTION("There are unconnected endpoints");
      }
    }

    if (!sg.player) {
      EXCEPTION("SpatialSystem must contain the player");
    };

    spatialSystem.connectZones();
    renderSystem.connectRegions();

    return true;
  }
  else {
    return false;
  }
}

//===========================================
// GeometryFactory::constructRegion
//===========================================
bool GeometryFactory::constructRegion(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (parentId == -1) {
    return constructRootRegion(obj);
  }
  else {
    return constructRegion_r(entityId, obj, parentId, parentTransform);
  }
}

//===========================================
// GeometryFactory::GeometryFactory
//===========================================
GeometryFactory::GeometryFactory(RootFactory& rootFactory, EntityManager& entityManager,
  AudioService& audioService, TimeService& timeService)
  : m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_audioService(audioService),
    m_timeService(timeService) {}

//===========================================
// GeometryFactory::types
//===========================================
const set<string>& GeometryFactory::types() const {
  static const set<string> types{
    "region",
    "join",
    "portal",
    "wall",
    "wall_decal",
    "floor_decal",
    "path"
  };

  return types;
}

//===========================================
// GeometryFactory::constructObject
//===========================================
bool GeometryFactory::constructObject(const string& type, entityId_t entityId,
  parser::Object& obj, entityId_t parentId, const Matrix& parentTransform) {

  if (type == "region") {
    return constructRegion(entityId, obj, parentId, parentTransform);
  }
  else if (type == "join") {
    return constructBoundaries(entityId, obj, parentId, parentTransform);
  }
  else if (type == "portal") {
    return constructPortal(obj, parentId, parentTransform);
  }
  else if (type == "wall") {
    return constructWalls(obj, parentId, parentTransform);
  }
  else if (type == "wall_decal") {
    return constructWallDecal(entityId, obj, parentId, parentTransform);
  }
  else if (type == "floor_decal") {
    return constructFloorDecal(entityId, obj, parentId, parentTransform);
  }
  else if (type == "path") {
    return constructPath(entityId, obj, parentId, parentTransform);
  }

  return false;
}
