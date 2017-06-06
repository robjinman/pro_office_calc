#include <string>
#include <sstream>
#include <tinyxml2.h>
#include "exception.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp"


namespace parser {


using std::list;
using std::string;
using std::map;
using std::pair;
using std::istream;
using std::stringstream;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;


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
    else if (s == "z") {
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
    ASSERT_EQ(comma, ',');
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
      ASSERT(ss.eof());
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
// parseKvpString
//===========================================
pair<string, string> parseKvpString(const string& s) {
  stringstream ss(s);
  string key;
  std::getline(ss, key, '=');
  string val;
  ss >> val;

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
      ASSERT_EQ(string(tspan->Name()), "tspan");

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
  ASSERT_EQ(buf, "translate(");

  Matrix m;
  char comma;
  ss >> m.tx >> comma;
  ASSERT_EQ(comma, ',');
  ss >> m.ty >> buf;
  ASSERT_EQ(buf, ")");

  return m;
}

//===========================================
// parseMatrixTransform
//===========================================
Matrix parseMatrixTransform(const string& s) {
  stringstream ss(s);

  string buf(7, '\0');
  ss.read(&buf[0], 7);
  ASSERT_EQ(buf, "matrix(");

  Matrix m;
  char comma;
  ss >> m[0][0] >> comma;
  ASSERT_EQ(comma, ',');
  ss >> m[1][0] >> comma;
  ASSERT_EQ(comma, ',');
  ss >> m[0][1] >> comma;
  ASSERT_EQ(comma, ',');
  ss >> m[1][1] >> comma;
  ASSERT_EQ(comma, ',');
  ss >> m[0][2] >> comma;
  ASSERT_EQ(comma, ',');
  ss >> m[1][2] >> buf;
  ASSERT_EQ(buf, ")");

  return m;
}

//===========================================
// parseTransform
//===========================================
Matrix parseTransform(const string& s) {
  ASSERT(s.length() > 0);

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

    if (tag == "path") {
      constructPath(*e, path);

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
// constructObject
//===========================================
Object constructObject(const XMLElement& node) {
  Object obj;

  extractKvPairs(node, obj.dict);
  extractGeometry(node, obj.path, obj.transform);

  return obj;
}

//===========================================
// parse
//===========================================
list<Object> parse(const string& file) {
  list<Object> objects;

  XMLDocument doc;
  doc.LoadFile(file.c_str());

  XMLElement* root = doc.FirstChildElement("svg");
  XMLElement* e = root->FirstChildElement();

  while (e != nullptr) {
    string tag(e->Name());

    if (tag == "g") {
      objects.push_back(constructObject(*e));
    }

    e = e->NextSiblingElement();
  }

  return objects;
}


}
