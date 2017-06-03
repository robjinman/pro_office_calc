#include <string>
#include <sstream>
#include <tinyxml2.h>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::unique_ptr;
using std::string;
using std::stringstream;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;


static LineSegment* createLineSegment(const XMLElement& e) {
  string data = e.Attribute("d");

  LineSegment* lseg = new LineSegment;
  char op, comma;

  stringstream ss;
  ss << data;

  ss >> op;
  ASSERT(op, 'm');

  ss >> lseg->A.x >> comma;
  ASSERT(comma, ',');

  ss >> lseg->A.y;
  ss >> lseg->B.x >> comma;
  ASSERT(comma, ',');

  ss >> lseg->B.y;

  DBG_PRINT(*lseg << "\n");

  return lseg;
}

static Matrix parseTransform(const std::string& str) {
  stringstream ss(str);

  string buf(7, '\0');
  ss.read(&buf[0], 7);
  ASSERT(buf, "matrix(");

  Matrix m;
  char comma;
  ss >> m[0][0] >> comma;
  ASSERT(comma, ',');
  ss >> m[1][0] >> comma;
  ASSERT(comma, ',');
  ss >> m[0][1] >> comma;
  ASSERT(comma, ',');
  ss >> m[1][1] >> comma;
  ASSERT(comma, ',');
  ss >> m[0][2] >> comma;
  ASSERT(comma, ',');
  ss >> m[1][2] >> buf;
  ASSERT(buf, ")");

  return m;
}

static Camera* createCamera(const XMLElement& e) {
  string trans = e.Attribute("transform");

  Matrix m = parseTransform(trans);
  m.tx += std::stod(e.Attribute("x"));
  m.ty += std::stod(e.Attribute("y"));

  Camera* camera = new Camera;
  camera->pos.x = m.tx;
  camera->pos.y = m.ty;
  camera->angle = m.a();

  return camera;
}

void Scene::addFromSvgElement(const XMLElement& e) {
  string tag(e.Name());

  if (tag == "path") {
    walls.push_back(unique_ptr<LineSegment>(createLineSegment(e)));
  }
  else if (tag == "text") {
    const XMLElement* ch = e.FirstChildElement();
    ASSERT(std::string(ch->Name()), "tspan");

    if (std::string(ch->GetText()) == "<") {
      camera.reset(createCamera(e));
      //DBG_PRINT(camera->matrix << "\n");
    }
  }
}

static void dbg_populateScene(Scene& scene) {
  scene.viewport.x = 8.0;
  scene.viewport.y = 8.0;
  scene.camera.reset(new Camera);
  scene.camera->pos.x = 0.0;
  scene.camera->pos.y = 0.0;
  scene.camera->angle = DEG_TO_RAD(45.0);
  scene.wallHeight = 10.0;

  scene.walls.push_back(unique_ptr<LineSegment>(new LineSegment(Point(-50.0, -50.0), Point(50.0, -50.0))));
  scene.walls.push_back(unique_ptr<LineSegment>(new LineSegment(Point(50.0, -50.0), Point(51.0, 50.0))));
  scene.walls.push_back(unique_ptr<LineSegment>(new LineSegment(Point(51.0, 50.0), Point(-49.0, 50.0))));
  scene.walls.push_back(unique_ptr<LineSegment>(new LineSegment(Point(-49.0, 50.0), Point(-50.0, -50.0))));
}


Scene::Scene(const std::string& mapFilePath) {
  dbg_populateScene(*this);
  /*
  XMLDocument doc;
  doc.LoadFile(mapFilePath.c_str());

  XMLElement* root = doc.FirstChildElement("svg");
  XMLElement* e = root->FirstChildElement();

  while (e != nullptr) {
    addFromSvgElement(*e);
    e = e->NextSiblingElement();
  }*/
}
