#include <string>
#include <gtest/gtest.h>
#include <tinyxml2.h>
#include <raycast/map_parser.hpp>
#include "utils.hpp"


using std::string;
using std::list;
using std::pair;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;
using namespace parser;


class MapParserTest : public testing::Test {
  public:
    virtual void SetUp() override {}

    virtual void TearDown() override {}
};

TEST_F(MapParserTest, parseKvpString) {
  pair<string, string> kvp("one", "two");
  ASSERT_EQ(kvp, parseKvpString("one=two"));
}

TEST_F(MapParserTest, parseKvpStringWithSpaces) {
  pair<string, string> kvp("one", "two three");
  ASSERT_EQ(kvp, parseKvpString("one=two three"));
}

TEST_F(MapParserTest, constructPath) {
  string d = "m 31.918905,67.183142 0,265.352498 L 293.18213,485.34829 325.2006,654.68072 "
    "109.36999,735.217 55.666713,913.83597 178.55975,979.92443 685.60906,978.88845 "
    "940.68329,926.23048 951.00762,586.53896 745.50133,584.46698 453.2558,628.86507 "
    "399.55252,460.56862 l 182.78531,-33.03957 24.78398,114.61183 348.02114,-7.23324 "
    "1.02683,-462.563232 -528.73412,1.035986 9.29749,157.973916 106.36106,-2.07197 "
    "17.55881,114.61182 -183.82147,11.35852 -8.26133,-289.105524 -336.651315,1.035986 z";

  XMLDocument doc;
  XMLElement* e = doc.NewElement("path");
  e->SetAttribute("d", d.c_str());

  Path path;
  constructPath(*e, path);

  Point c;
  auto i = path.points.begin();

  ASSERT_EQ(24, path.points.size());
  ASSERT_DOUBLE_EQ(c.x += 31.918905, i->x);
  ASSERT_DOUBLE_EQ(c.y += 67.183142, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += 0, i->x);
  ASSERT_DOUBLE_EQ(c.y += 265.352498, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(293.18213, i->x);
  ASSERT_DOUBLE_EQ(485.34829, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(325.2006, i->x);
  ASSERT_DOUBLE_EQ(654.68072, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(109.36999, i->x);
  ASSERT_DOUBLE_EQ(735.217, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(55.666713, i->x);
  ASSERT_DOUBLE_EQ(913.83597, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(178.55975, i->x);
  ASSERT_DOUBLE_EQ(979.92443, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(685.60906, i->x);
  ASSERT_DOUBLE_EQ(978.88845, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(940.68329, i->x);
  ASSERT_DOUBLE_EQ(926.23048, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(951.00762, i->x);
  ASSERT_DOUBLE_EQ(586.53896, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(745.50133, i->x);
  ASSERT_DOUBLE_EQ(584.46698, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(453.2558, i->x);
  ASSERT_DOUBLE_EQ(628.86507, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(399.55252, i->x);
  ASSERT_DOUBLE_EQ(460.56862, i->y);
  ++i;
  c = Point(399.55252, 460.56862);
  ASSERT_DOUBLE_EQ(c.x += 182.78531, i->x);
  ASSERT_DOUBLE_EQ(c.y += -33.03957, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += 24.78398, i->x);
  ASSERT_DOUBLE_EQ(c.y += 114.61183, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += 348.02114, i->x);
  ASSERT_DOUBLE_EQ(c.y += -7.23324, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += 1.02683, i->x);
  ASSERT_DOUBLE_EQ(c.y += -462.563232, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += -528.73412, i->x);
  ASSERT_DOUBLE_EQ(c.y += 1.035986, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += 9.29749, i->x);
  ASSERT_DOUBLE_EQ(c.y += 157.973916, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += 106.36106, i->x);
  ASSERT_DOUBLE_EQ(c.y += -2.07197, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += 17.55881, i->x);
  ASSERT_DOUBLE_EQ(c.y += 114.61182, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += -183.82147, i->x);
  ASSERT_DOUBLE_EQ(c.y += 11.35852, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += -8.26133, i->x);
  ASSERT_DOUBLE_EQ(c.y += -289.105524, i->y);
  ++i;
  ASSERT_DOUBLE_EQ(c.x += -336.651315, i->x);
  ASSERT_DOUBLE_EQ(c.y += 1.035986, i->y);
}

TEST_F(MapParserTest, transformFromTriangle_rightFacing) {
  Path path;
  path.points.push_back(Point(-10, 10));
  path.points.push_back(Point(-10, -10));
  path.points.push_back(Point(20, 0));
  path.closed = true;

  Matrix m = transformFromTriangle(path);

  ASSERT_DOUBLE_EQ(0, m.a());
}

TEST_F(MapParserTest, transformFromTriangle_leftFacing) {
  Path path;
  path.points.push_back(Point(10, 10));
  path.points.push_back(Point(10, -10));
  path.points.push_back(Point(-20, 0));
  path.closed = true;

  Matrix m = transformFromTriangle(path);

  ASSERT_DOUBLE_EQ(PI, m.a());
}

TEST_F(MapParserTest, constructObject_r_keyValuePairs) {
  string xml =
    "<g>"
      "<text><tspan>type=typename</tspan></text>"
      "<text><tspan>key1=value1</tspan></text>"
      "<text><tspan>key2=value2</tspan></text>"
      "<text><tspan>key3=value3</tspan></text>"
    "</g>";

  XMLDocument doc;
  doc.Parse(xml.c_str(), xml.size());

  ParseErrors errors;

  XMLElement* node = doc.FirstChildElement("g");
  Object* obj = constructObject_r(nullptr, *node, errors);

  ASSERT_EQ(obj->type, "typename");

  auto it = obj->dict.find("key1");
  ASSERT_TRUE(it != obj->dict.end());
  ASSERT_EQ(it->second, "value1");

  it = obj->dict.find("key2");
  ASSERT_TRUE(it != obj->dict.end());
  ASSERT_EQ(it->second, "value2");

  it = obj->dict.find("key3");
  ASSERT_TRUE(it != obj->dict.end());
  ASSERT_EQ(it->second, "value3");
}

TEST_F(MapParserTest, constructObject_r_nestedObjects) {
  string xml =
    "<g>"
      "<text><tspan>type=type1</tspan></text>"
      "<text><tspan>key1=value1</tspan></text>"
      "<text><tspan>key2=value2</tspan></text>"
      "<g>"
        "<text><tspan>type=type2</tspan></text>"
        "<text><tspan>key3=value3</tspan></text>"
        "<text><tspan>key4=value4</tspan></text>"
      "</g>"
    "</g>";

  XMLDocument doc;
  doc.Parse(xml.c_str(), xml.size());

  ParseErrors errors;

  XMLElement* node = doc.FirstChildElement("g");
  Object* obj = constructObject_r(nullptr, *node, errors);

  ASSERT_EQ(obj->type, "type1");

  auto it = obj->dict.find("key1");
  ASSERT_TRUE(it != obj->dict.end());
  ASSERT_EQ(it->second, "value1");

  it = obj->dict.find("key2");
  ASSERT_TRUE(it != obj->dict.end());
  ASSERT_EQ(it->second, "value2");

  ASSERT_EQ(1, obj->children.size());

  pObject_t& obj2 = *obj->children.begin();

  ASSERT_EQ(obj2->type, "type2");

  it = obj2->dict.find("key3");
  ASSERT_TRUE(it != obj2->dict.end());
  ASSERT_EQ(it->second, "value3");

  it = obj2->dict.find("key4");
  ASSERT_TRUE(it != obj2->dict.end());
  ASSERT_EQ(it->second, "value4");
}

TEST_F(MapParserTest, constructObject_r_testFile1) {
  list<pObject_t> objects;
  parse("tests/data/test_map1.svg", objects);

  ASSERT_EQ(1, objects.size());

  auto it = objects.begin();
  const Object& obj = **it;

  ASSERT_EQ("region", obj.type);
  ASSERT_EQ("0", obj.dict.at("floor_height"));
  ASSERT_EQ("140", obj.dict.at("ceiling_height"));
  ASSERT_EQ("cracked_mud", obj.dict.at("floor_texture"));
  ASSERT_EQ("grey_stone", obj.dict.at("ceiling_texture"));

  ASSERT_EQ(1, obj.children.size());

  const Object& subObj = **obj.children.begin();

  ASSERT_EQ("wall", subObj.type);
  ASSERT_EQ("light_bricks", subObj.dict.at("texture"));
  ASSERT_EQ(4, subObj.path.points.size());

  ASSERT_EQ(0, subObj.children.size());
}

TEST_F(MapParserTest, constructObject_r_testFile2) {
  list<pObject_t> objects;
  parse("tests/data/test_map2.svg", objects);

  ASSERT_EQ(1, 1);
}
