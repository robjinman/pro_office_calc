#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_MAP_PARSER_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_MAP_PARSER_HPP__


#include <list>
#include <vector>
#include <map>
#include <memory>
#include <tinyxml2.h>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"


namespace parser {


struct Path {
  std::vector<Point> points;
  bool closed = false;
};

class Object;

typedef std::unique_ptr<Object> pObject_t;

struct Object {
  Matrix transform;
  Path path;
  std::map<std::string, std::string> dict;
  std::list<pObject_t> children;
};

void parse(const std::string& file, std::list<pObject_t>& objects);

struct PathStreamToken {
  enum { SET_RELATIVE, SET_ABSOLUTE, CLOSE_PATH, POINT } kind;
  Point p;
};

Matrix transformFromTriangle(const Path& path);
PathStreamToken getNextToken(std::istream& is);
void constructPath(const tinyxml2::XMLElement& e, Path& path);
std::pair<std::string, std::string> parseKvpString(const std::string& s);
void extractKvPairs(const tinyxml2::XMLElement& e, std::map<std::string, std::string>& kv);
Matrix parseTranslateTransform(const std::string& s);
Matrix parseMatrixTransform(const std::string& s);
Matrix parseTransform(const std::string& s);
void extractGeometry(const tinyxml2::XMLElement& node, Path& path, Matrix& transform);
Object* constructObject_r(const tinyxml2::XMLElement& node);


}


const std::string& getValue(const std::map<std::string, std::string>& m, const std::string& key);
const std::string& getValue(const std::map<std::string, std::string>& m, const std::string& key,
  const std::string& default_);


#endif
