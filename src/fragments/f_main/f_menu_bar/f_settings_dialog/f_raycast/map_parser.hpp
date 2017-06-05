#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_MAP_PARSER_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_MAP_PARSER_HPP__


#include <list>
#include <map>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"


namespace parser {


struct Path {
  std::list<Point> points;
  bool closed = false;
};

struct Object {
  Matrix transform;
  Path path;
  std::map<std::string, std::string> dict;
};

std::list<Object> parse(const std::string& file);

struct PathStreamToken {
  enum { SET_RELATIVE, SET_ABSOLUTE, CLOSE_PATH, POINT } kind;
  Point p;
};

PathStreamToken getNextToken(std::istream& is);
void constructPath(const tinyxml2::XMLElement& e, Path& path);
std::pair<std::string, std::string> parseKvpString(const std::string& s);
void extractKvPairs(const tinyxml2::XMLElement& e, std::map<std::string, std::string>& kv);
Matrix parseTranslateTransform(const std::string& s);
Matrix parseMatrixTransform(const std::string& s);
Matrix parseTransform(const std::string& s);
void extractGeometry(const tinyxml2::XMLElement& node, Path& path, Matrix& transform);
Object constructObject(const tinyxml2::XMLElement& node);


}


#endif
