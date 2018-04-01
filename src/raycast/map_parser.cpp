#include <string>
#include <sstream>
#include "exception.hpp"
#include "raycast/map_parser.hpp"
#include "utils.hpp"


using std::list;
using std::string;
using std::map;
using std::pair;
using std::istream;
using std::stringstream;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;


namespace parser {


//===========================================
// isTriangle
//===========================================
static bool isTriangle(const Path& path) {
  return path.points.size() == 3 && path.closed;
}

//===========================================
// transformFromTriangle
//===========================================
Matrix transformFromTriangle(const Path& path) {
  if (!isTriangle(path)) {
    EXCEPTION("Path is not a triangle");
  }

  Point centre = (path.points[0] + path.points[1] + path.points[2]) / 3.0;
  Point mostDistantPoint = centre;

  for (int i = 0; i < 3; ++i) {
    if (distance(path.points[i], centre) > distance(mostDistantPoint, centre)) {
      mostDistantPoint = path.points[i];
    }
  }

  Vec2f v = mostDistantPoint - centre;
  double a = atan2(v.y, v.x);

  return Matrix(a, centre);
}

//===========================================
// getNextToken
//===========================================
PathStreamToken getNextToken(istream& is) {
  PathStreamToken result;

  string s;
  is >> s;

  if (s.length() == 1) {
    if (s == "m" || s == "l") {
      result.kind = PathStreamToken::SET_RELATIVE;
    }
    else if (s == "M" || s == "L") {
      result.kind = PathStreamToken::SET_ABSOLUTE;
    }
    else if (s == "z" || s == "Z") {
      result.kind = PathStreamToken::CLOSE_PATH;
    }
    else {
      EXCEPTION("Unknown SVG path operator '" << s << "'");
    }
  }
  else {
    result.kind = PathStreamToken::POINT;

    char comma = '_';
    stringstream ss(s);
    ss >> result.p.x;
    ss >> comma;
    PC_ASSERT_EQ(comma, ',');
    ss >> result.p.y;
  }

  return result;
}

//===========================================
// constructPath
//===========================================
void constructPath(const XMLElement& e, Path& path) {
  string data = e.Attribute("d");
  stringstream ss(data);

  Point cursor;
  bool relative = false;

  while (!ss.eof()) {
    PathStreamToken result = getNextToken(ss);

    if (result.kind == PathStreamToken::SET_RELATIVE) {
      relative = true;
    }
    else if (result.kind == PathStreamToken::SET_ABSOLUTE) {
      relative = false;
    }
    else if (result.kind == PathStreamToken::CLOSE_PATH) {
      path.closed = true;
      PC_ASSERT(ss.eof());
      break;
    }
    else if (result.kind == PathStreamToken::POINT) {
      Point p;

      if (relative) {
        p = cursor + result.p;
      }
      else {
        p = result.p;
      }

      path.points.push_back(p);
      cursor = p;
    }
  }
}

//===========================================
// constructPathFromRect
//===========================================
void constructPathFromRect(const XMLElement& e, Path& path) {
  double x = std::stod(e.Attribute("x"));
  double y = std::stod(e.Attribute("y"));
  double w = std::stod(e.Attribute("width"));
  double h = std::stod(e.Attribute("height"));

  path.points.push_back(Point(x, y));
  path.points.push_back(Point(x + w, y));
  path.points.push_back(Point(x + w, y + h));
  path.points.push_back(Point(x, y + h));
  path.closed = true;
}

//===========================================
// parseKvpString
//===========================================
pair<string, string> parseKvpString(const string& s) {
  stringstream ss(s);
  string key, val;

  std::getline(ss, key, '=');
  std::getline(ss, val, '=');

  return std::make_pair(key, val);
}

//===========================================
// extractKvPairs
//===========================================
void extractKvPairs(const XMLElement& node, map<string, string>& kv) {
  const XMLElement* e = node.FirstChildElement();
  while (e != nullptr) {
    string tag(e->Name());

    if (tag == "text") {
      const XMLElement* tspan = e->FirstChildElement();
      PC_ASSERT_EQ(string(tspan->Name()), "tspan");

      kv.insert(parseKvpString(tspan->GetText()));
    }

    e = e->NextSiblingElement();
  }
}

//===========================================
// parseTranslateTransform
//===========================================
Matrix parseTranslateTransform(const string& s) {
  stringstream ss(s);

  string buf(10, '\0');
  ss.read(&buf[0], 10);
  PC_ASSERT_EQ(buf, "translate(");

  Matrix m;
  char comma;
  ss >> m[0][2] >> comma;
  PC_ASSERT_EQ(comma, ',');
  ss >> m[1][2] >> buf;
  PC_ASSERT_EQ(buf, ")");

  return m;
}

//===========================================
// parseMatrixTransform
//===========================================
Matrix parseMatrixTransform(const string& s) {
  stringstream ss(s);

  string buf(7, '\0');
  ss.read(&buf[0], 7);
  PC_ASSERT_EQ(buf, "matrix(");

  Matrix m;
  char comma;
  ss >> m[0][0] >> comma;
  PC_ASSERT_EQ(comma, ',');
  ss >> m[1][0] >> comma;
  PC_ASSERT_EQ(comma, ',');
  ss >> m[0][1] >> comma;
  PC_ASSERT_EQ(comma, ',');
  ss >> m[1][1] >> comma;
  PC_ASSERT_EQ(comma, ',');
  ss >> m[0][2] >> comma;
  PC_ASSERT_EQ(comma, ',');
  ss >> m[1][2] >> buf;
  PC_ASSERT_EQ(buf, ")");

  return m;
}

//===========================================
// parseTransform
//===========================================
Matrix parseTransform(const string& s) {
  PC_ASSERT(s.length() > 0);

  if (s[0] == 't') {
    return parseTranslateTransform(s);
  }
  else if (s[0] == 'm') {
    return parseMatrixTransform(s);
  }

  EXCEPTION("Error parsing unknown transform");
}

//===========================================
// extractGeometry
//===========================================
void extractGeometry(const XMLElement& node, Path& path, Matrix& transform) {
  Matrix groupM;
  Matrix pathM;

  const char* trans = node.Attribute("transform");
  if (trans) {
    groupM = parseTransform(trans);
  }

  const XMLElement* e = node.FirstChildElement();
  while (e != nullptr) {
    string tag(e->Name());

    if (tag == "path" || tag == "rect") {
      if (tag == "path") {
        constructPath(*e, path);
      }
      else if (tag == "rect") {
        constructPathFromRect(*e, path);
      }

      const char* trans = e->Attribute("transform");
      if (trans) {
        pathM = parseTransform(trans);
      }

      break;
    }

    e = e->NextSiblingElement();
  }

  transform = groupM * pathM;
}

//===========================================
// constructObject_r
//===========================================
Object* constructObject_r(const XMLElement& node) {
  Object* obj = new Object;

  extractKvPairs(node, obj->dict);
  if (obj->dict.count("type") == 0) {
    EXCEPTION("Object has no type key");
  }
  obj->type = obj->dict.at("type");
  obj->dict.erase("type");

  extractGeometry(node, obj->path, obj->transform);

  const XMLElement* e = node.FirstChildElement();
  while (e != nullptr) {
    string tag(e->Name());

    if (tag == "g") {
      obj->children.push_back(pObject_t(constructObject_r(*e)));
    }

    e = e->NextSiblingElement();
  }

  return obj;
}

//===========================================
// parse
//===========================================
void parse(const string& file, list<pObject_t>& objects) {
  XMLDocument doc;
  doc.LoadFile(file.c_str());

  if (doc.Error()) {
    EXCEPTION("Error parsing map file " << file << "; ErrorID=" << doc.ErrorID());
  }

  XMLElement* root = doc.FirstChildElement("svg");
  XMLElement* e = root->FirstChildElement();

  while (e != nullptr) {
    string tag(e->Name());

    if (tag == "g") {
      objects.push_back(pObject_t(constructObject_r(*e)));
    }

    e = e->NextSiblingElement();
  }
}

//===========================================
// firstObjectOfType
//===========================================
Object* firstObjectOfType(const list<pObject_t>& objects, const string& type) {
  for (auto& pObj : objects) {
    if (pObj->type == type) {
      return pObj.get();
    }
  }

  return nullptr;
}


}


//===========================================
// getValue
//===========================================
const string& getValue(const map<string, string>& m, const string& key) {
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
const string& getValue(const map<string, string>& m, const string& key, const string& default_) {
  if (m.find(key) != m.end()) {
    return m.at(key);
  }
  else {
    return default_;
  }
}

//===========================================
// makeIdForObj
//===========================================
entityId_t makeIdForObj(const parser::Object& obj) {
  if (contains<string>(obj.dict, "name")) {
    return Component::getIdFromString(getValue(obj.dict, "name"));
  }
  else {
    return Component::getNextId();
  }
}
