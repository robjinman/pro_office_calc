#include <string>
#include <gtest/gtest.h>
#include <tinyxml2.h>
#include <fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp>


using std::string;
using std::pair;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;


class MapParserTest : public testing::Test {
  public:
    virtual void SetUp() override {}

    virtual void TearDown() override {}
};

TEST_F(MapParserTest, parseKvpString) {
  pair<string, string> kvp("one", "two");
  ASSERT_EQ(kvp, parser::parseKvpString("one=two"));
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

  parser::Path path;
  parser::constructPath(*e, path);

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
