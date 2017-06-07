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
static void constructWalls(const parser::Object& obj, list<Wall>& walls) {
  for (auto it = std::next(obj.path.points.begin()); it != obj.path.points.end(); ++it) {
    Wall wall;

    wall.lseg.A = *std::prev(it);
    wall.lseg.B = *it;
    transform(wall.lseg, obj.transform);

    wall.texture = obj.dict.at("texture");

    walls.push_back(wall);
  }
}

//===========================================
// isTriangle
//===========================================
static bool isTriangle(const parser::Path& path) {
  return path.points.size() == 3 && path.closed;
}

//===========================================
// transformFromTriangle
//===========================================
static Matrix transformFromTriangle(const parser::Path& path) {
  if (!isTriangle(path)) {
    EXCEPTION("Path is not a triangle");
  }

  double furthestNearest = 0.0;
  Point mostDistantPoint;

  for (int i = 0; i < 3; ++i) {
    const Point& P = path.points[i];
    const Point& A = path.points[(i + 1) % 3];
    const Point& B = path.points[(i + 2) % 3];

    double nearest = smallest(distance(P, A), distance(P, B));
    if (nearest > furthestNearest) {
      furthestNearest = nearest;
      mostDistantPoint = P;
    }
  }

  Point centre = (path.points[0] + path.points[1] + path.points[2]) / 3.0;

  Vec2f v = mostDistantPoint - centre;
  double a = atan(v.y / v.x) - PI;

  return Matrix(a, centre);
}

//===========================================
// constructCamera
//===========================================
static Camera constructCamera(const parser::Object& obj) {
  Camera camera;
  camera.setTransform(transformFromTriangle(obj.path));

  return camera;
}

//===========================================
// Scene::addObject
//===========================================
void Scene::addObject(const parser::Object& obj) {
  if (obj.dict.at("type") == "wall") {
    constructWalls(obj, walls);
  }
  else if (obj.dict.at("type") == "camera") {
    camera = constructCamera(obj);
  }
}

//===========================================
// Scene::Scene
//===========================================
Scene::Scene(const std::string& mapFilePath) {
  list<parser::Object> objects = parser::parse(mapFilePath);
  for (auto it = objects.begin(); it != objects.end(); ++it) {
    addObject(*it);
  }

  viewport.x = 10.0;
  viewport.y = 10.0;
  wallHeight = 100.0;

  textures["light_bricks"] = QPixmap("data/light_bricks.png");
  textures["dark_bricks"] = QPixmap("data/dark_bricks.png");
}
