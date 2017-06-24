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
static void constructWalls(const parser::Object& obj, list<unique_ptr<Wall>>& walls) {
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
    wall->lseg = transform(wall->lseg, obj.transform);

    wall->texture = obj.dict.at("texture");

    walls.push_back(unique_ptr<Wall>(wall));
  }
}

//===========================================
// constructCamera
//===========================================
static Camera* constructCamera(const parser::Object& obj) {
  Camera* camera = new Camera;
  camera->setTransform(obj.transform * transformFromTriangle(obj.path));

  return camera;
}

//===========================================
// constructSprite
//===========================================
static Sprite* constructSprite(const parser::Object& obj) {
  if (obj.dict.at("subtype") == "bad_guy") {
    BadGuy* sprite = new BadGuy;
    Matrix m = transformFromTriangle(obj.path);
    sprite->setTransform(obj.transform * m);

    return sprite;
  }
  else if (obj.dict.at("subtype") == "ammo") {
    Ammo* sprite = new Ammo;
    Matrix m = transformFromTriangle(obj.path);
    sprite->setTransform(obj.transform * m);

    return sprite;
  }

  EXCEPTION("Error constructing sprite of unknown type");
}

static void buildPolygon(ConvexRegion& region) {
  region.polygon.points.clear();
  Polygon polyCpy;

  for (auto it = region.edges.begin(); it != region.edges.end(); ++it) {
    const Edge& edge = **it;

    // If the line segments are connected, lseg.A and lseg.B should yield
    // the same polygon
    region.polygon.points.push_back(edge.lseg.A);
    polyCpy.points.push_back(edge.lseg.B);
  }

  if (!polygonsEqual(region.polygon, polyCpy, 0.0001)) {
    EXCEPTION("Error constructing polygon; Line segments not connected\n");
  }
}

static void populateScene(Scene& scene) {
  scene.camera.reset(new Camera);
  scene.camera->pos = Point(30, 30);
  scene.camera->angle = DEG_TO_RAD(45);





  ConvexRegion* rootRegion = new ConvexRegion;
  ConvexRegion* region1 = new ConvexRegion;
  ConvexRegion* region2 = new ConvexRegion;

  rootRegion->children.push_back(unique_ptr<ConvexRegion>(region1));
  rootRegion->children.push_back(unique_ptr<ConvexRegion>(region2));

  Wall* wall = new Wall;
  scene.edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(0, 0), Point(200, 1));
  wall->texture = "light_bricks";
  region1->edges.push_back(wall);

  JoiningEdge* je = new JoiningEdge;
  scene.edges.push_back(std::unique_ptr<JoiningEdge>(je));
  je->lseg = LineSegment(Point(200, 1), Point(201, 201));
  je->topTexture = "light_bricks";
  je->bottomTexture = "dark_bricks";
  region1->edges.push_back(je);

  wall = new Wall;
  scene.edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(201, 201), Point(1, 200));
  wall->texture = "light_bricks";
  region1->edges.push_back(wall);

  wall = new Wall;
  scene.edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(1, 200), Point(0, 0));
  wall->texture = "light_bricks";
  region1->edges.push_back(wall);

  wall = new Wall;
  scene.edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(200, 1), Point(400, 2));
  wall->texture = "light_bricks";
  region2->edges.push_back(wall);

  wall = new Wall;
  scene.edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(400, 2), Point(401, 202));
  wall->texture = "light_bricks";
  region2->edges.push_back(wall);

  wall = new Wall;
  scene.edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(401, 202), Point(201, 201));
  wall->texture = "light_bricks";
  region2->edges.push_back(wall);

  region2->edges.push_back(je);
  je->regionA = region1;
  je->regionB = region2;




/*
  ConvexRegion* region = new ConvexRegion;

  Wall* wall = new Wall;
  scene.edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(1, 1), Point(200, 1));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  wall = new Wall;
  scene.edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(200, 1), Point(200, 200));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  wall = new Wall;
  scene.edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(200, 200), Point(0, 200));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  wall = new Wall;
  scene.edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(0, 200), Point(1, 1));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  buildPolygon(*region);
*/
  scene.rootRegion.reset(rootRegion);
}

//===========================================
// Scene::Scene
//===========================================
Scene::Scene(const string& mapFilePath) {
  /*
  list<parser::Object> objects = parser::parse(mapFilePath);
  for (auto it = objects.begin(); it != objects.end(); ++it) {
    addObject(*it);
  }*/

  populateScene(*this);

  viewport.x = 10.0 * 320.0 / 240.0; // TODO: Read from map file
  viewport.y = 10.0;
  wallHeight = 100.0;
  currentRegion = rootRegion.get();

  camera->height = wallHeight / 2;
  camera->F = computeF(viewport.x, camera->hFov);

  textures["light_bricks"] = QImage("data/light_bricks.png");
  textures["dark_bricks"] = QImage("data/dark_bricks.png");
  textures["floor"] = QImage("data/floor.png");
  textures["ceiling"] = QImage("data/ceiling.png");
  textures["ammo"] = QImage("data/ammo.png");
  textures["bad_guy"] = QImage("data/bad_guy.png");
}

//===========================================
// Scene::addObject
//===========================================
void Scene::addObject(const parser::Object& obj) {
  if (obj.dict.at("type") == "wall") {
    constructWalls(obj, walls);
  }
  else if (obj.dict.at("type") == "camera") {
    camera.reset(constructCamera(obj));
  }
  else if (obj.dict.at("type") == "sprite") {
    sprites.push_back(unique_ptr<Sprite>(constructSprite(obj)));
  }
}
