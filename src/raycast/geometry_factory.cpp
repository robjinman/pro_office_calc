#include <list>
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


const double SNAP_DISTANCE = 4.0;


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
// GeometryFactory::constructWallDecal
//===========================================
bool GeometryFactory::constructWallDecal(entityId_t entityId, const parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem&>(ComponentKind::C_RENDER);

  const CEdge& edge = dynamic_cast<const CEdge&>(spatialSystem.getComponent(parentId));
  const LineSegment& wall = edge.lseg;

  Point A = parentTransform * obj.transform * obj.path.points[0];
  Point B = parentTransform * obj.transform * obj.path.points[1];

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

  double r = std::stod(getValue(obj.dict, "aspect_ratio"));
  Size size(w, w / r);

  double y = std::stod(getValue(obj.dict, "y"));
  Point pos(a, y);

  string texture = getValue(obj.dict, "texture", "default");

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  CVRect* vRect = new CVRect(entityId, parentId, size);
  vRect->pos = pos;

  spatialSystem.addComponent(pComponent_t(vRect));

  CWallDecal* decal = new CWallDecal(entityId, parentId);
  decal->texture = texture;

  renderSystem.addComponent(pComponent_t(decal));

  return true;
}

//===========================================
// GeometryFactory::constructWalls
//===========================================
bool GeometryFactory::constructWalls(const parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(parentId));
  CRegion& region = dynamic_cast<CRegion&>(renderSystem.getComponent(parentId));

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
      m_rootFactory.constructObject((*it)->type, -1, **it, entityId, m);
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
bool GeometryFactory::constructFloorDecal(entityId_t entityId, const parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  string texture = getValue(obj.dict, "texture", "default");

  Point pos = obj.path.points[0];
  Size size = obj.path.points[2] - obj.path.points[0];

  assert(size.x > 0);
  assert(size.y > 0);

  Matrix m(0, pos);

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  CHRect* hRect = new CHRect(entityId, parentId);
  hRect->size = size;
  hRect->transform = parentTransform * obj.transform * m;

  spatialSystem.addComponent(pComponent_t(hRect));

  CFloorDecal* decal = new CFloorDecal(entityId, parentId);
  decal->texture = texture;

  renderSystem.addComponent(pComponent_t(decal));

  return true;
}

//===========================================
// constructPlayer
//===========================================
bool GeometryFactory::constructPlayer(const parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  const double COLLISION_RADIUS = 10;

  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  AnimationSystem& animationSystem =
    m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);
  DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(parentId));

  const Size& viewport = renderSystem.rg.viewport;

  double tallness = std::stod(getValue(obj.dict, "tallness"));

  entityId_t bodyId = Component::getNextId();

  CVRect* body = new CVRect(bodyId, zone.entityId(), Size(COLLISION_RADIUS * 2, tallness));
  body->setTransform(parentTransform * obj.transform * transformFromTriangle(obj.path));
  body->zone = &zone;
  spatialSystem.addComponent(pCSpatial_t(body));

  Camera* camera = new Camera(viewport.x, DEG_TO_RAD(60), DEG_TO_RAD(50), *body);
  camera->height = tallness + zone.floorHeight;

  CDamage* damage = new CDamage(bodyId, 10, 10);
  damageSystem.addComponent(pCDamage_t(damage));

  Player* player = new Player(m_entityManager, m_audioService, tallness, unique_ptr<Camera>(camera),
    *body);
  player->sprite = Component::getNextId();
  player->crosshair = Component::getNextId();

  CEventHandler* takeDamage = new CEventHandler(bodyId);
  takeDamage->handlers.push_back(EventHandler{"entityDamaged", [=, &spatialSystem, &renderSystem,
    &viewport](const GameEvent& e) {

    DBG_PRINT("Player health: " << damage->health << "\n");

    if (player->red == -1) {
      player->red = Component::getNextId();

      double maxAlpha = 80;
      CColourOverlay* overlay = new CColourOverlay(player->red, QColor(200, 0, 0, maxAlpha),
        Point(0, 0), viewport);

      renderSystem.addComponent(pCRender_t(overlay));

      double duration = 0.33;
      int da = maxAlpha / (duration * m_timeService.frameRate);

      m_timeService.addTween(Tween{[=](long, double, double) -> bool {
        int alpha = overlay->colour.alpha() - da;
        overlay->colour.setAlpha(alpha);

        return alpha > 0;
      }, [&, player](long, double, double) {
        m_entityManager.deleteEntity(player->red);
        player->red = -1;
      }}, "redFade");
    }
  }});
  eventHandlerSystem.addComponent(pComponent_t(takeDamage));

  Size sz(0.5, 0.5);
  CImageOverlay* crosshair = new CImageOverlay(player->crosshair, "crosshair",
    viewport / 2 - sz / 2, sz);
  renderSystem.addComponent(pCRender_t(crosshair));

  CImageOverlay* sprite = new CImageOverlay(player->sprite, "gun", Point(viewport.x * 0.6, 0),
    Size(3, 3));
  sprite->texRect = QRectF(0, 0, 0.25, 1);
  renderSystem.addComponent(pCRender_t(sprite));

  CAnimation* shoot = new CAnimation(player->sprite);
  shoot->animations.insert(std::make_pair("shoot", Animation(m_timeService.frameRate, 0.4, {
    AnimationFrame{{
      QRectF(0.75, 0, 0.25, 1)
    }},
    AnimationFrame{{
      QRectF(0.5, 0, 0.25, 1)
    }},
    AnimationFrame{{
      QRectF(0.25, 0, 0.25, 1)
    }},
    AnimationFrame{{
      QRectF(0, 0, 0.25, 1)
    }}
  })));

  animationSystem.addComponent(pCAnimation_t(shoot));

  spatialSystem.sg.player.reset(player);

  return true;
}

//===========================================
// GeometryFactory::constructPortal
//===========================================
bool GeometryFactory::constructPortal(const parser::Object& obj, entityId_t parentId,
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
  edge->lseg = transform(edge->lseg, parentTransform * obj.transform);
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
bool GeometryFactory::constructBoundaries(const parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  Matrix m = parentTransform * obj.transform;

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

    snapEndpoint(m_endpoints, edge->lseg.A);
    snapEndpoint(m_endpoints, edge->lseg.B);

    edges.push_back(edge);

    spatialSystem.addComponent(pComponent_t(edge));

    CJoin* boundary = new CJoin(entityId, parentId, Component::getNextId());
    boundary->topTexture = getValue(obj.dict, "top_texture", "default");
    boundary->bottomTexture = getValue(obj.dict, "bottom_texture", "default");

    renderSystem.addComponent(pComponent_t(boundary));

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      m_rootFactory.constructObject((*it)->type, -1, **it, entityId, m);
    }
  }

  return true;
}

//===========================================
// GeometryFactory::constructRegion_r
//===========================================
bool GeometryFactory::constructRegion_r(entityId_t entityId, const parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  SceneGraph& sg = spatialSystem.sg;
  RenderGraph& rg = renderSystem.rg;

  CZone* parentZone = parentId == -1 ? nullptr
    : dynamic_cast<CZone*>(&spatialSystem.getComponent(parentId));

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

    Matrix transform = parentTransform * obj.transform;

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

    zone->floorHeight = contains<string>(obj.dict, "floor_height") ?
      std::stod(getValue(obj.dict, "floor_height")) : sg.defaults.floorHeight;

    zone->ceilingHeight = contains<string>(obj.dict, "ceiling_height") ?
      std::stod(getValue(obj.dict, "ceiling_height")) : sg.defaults.ceilingHeight;

    region->floorTexture = getValue(obj.dict, "floor_texture", rg.defaults.floorTexture);
    region->ceilingTexture =  getValue(obj.dict, "ceiling_texture", rg.defaults.ceilingTexture);

    for (auto it = obj.children.begin(); it != obj.children.end(); ++it) {
      const parser::Object& child = **it;

      m_rootFactory.constructObject(child.type, -1, child, entityId, transform);
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
bool GeometryFactory::constructRootRegion(const parser::Object& obj) {
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
bool GeometryFactory::constructRegion(entityId_t entityId, const parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

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
    "player",
    "region",
    "joining_edge",
    "portal",
    "wall",
    "wall_decal",
    "floor_decal"
  };

  return types;
}

//===========================================
// GeometryFactory::constructObject
//===========================================
bool GeometryFactory::constructObject(const string& type, entityId_t entityId,
  const parser::Object& obj, entityId_t parentId, const Matrix& parentTransform) {

  if (type == "player") {
    return constructPlayer(obj, parentId, parentTransform);
  }
  else if (type == "region") {
    return constructRegion(entityId, obj, parentId, parentTransform);
  }
  else if (type == "joining_edge") {
    return constructBoundaries(obj, parentId, parentTransform);
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

  return false;
}
