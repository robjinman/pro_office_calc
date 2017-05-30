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


static LineSegment* createLineSegment(const string& data) {
  LineSegment* lseg = new LineSegment;
  char op, comma;

  stringstream ss;
  ss << data;

  ss >> op;
  ASSERT(op == 'm');

  ss >> lseg->A.x >> comma;
  ASSERT(comma == ',');

  ss >> lseg->A.y;
  ss >> lseg->B.x >> comma;
  ASSERT(comma == ',');

  ss >> lseg->B.y;

  DBG_PRINT(*lseg << "\n");

  return lseg;
}

static Primitive* createPrimitiveFromSvgElement(XMLElement* e) {
  if (std::string(e->Name()) == "path") {
    return createLineSegment(e->Attribute("d"));
  }

  return nullptr;
}


Scene::Scene(const std::string& mapFilePath) {
  XMLDocument doc;
  doc.LoadFile(mapFilePath.c_str());

  XMLElement* root = doc.FirstChildElement("svg");
  XMLElement* group = root->FirstChildElement("g");
  XMLElement* e = group->FirstChildElement();

  while (e != nullptr) {
    Primitive* prim = createPrimitiveFromSvgElement(e);
    if (prim != nullptr) {
      primitives.push_back(unique_ptr<Primitive>(prim));
    }

    e = e->NextSiblingElement();
  }
}
