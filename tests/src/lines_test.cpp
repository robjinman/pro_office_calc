#include <gtest/gtest.h>
#include <fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp>


class LinesTest : public testing::Test {
  public:
    virtual void SetUp() override {}

    virtual void TearDown() override {}
};


TEST_F(LinesTest, lineSegmentIntersect) {
  LineSegment l0(Point(0, -2), Point(4, 10));
  LineSegment l1(Point(0, 8), Point(4, 0));

  Point p;
  ASSERT_TRUE(lineSegmentIntersect(l0, l1, p));
  ASSERT_DOUBLE_EQ(2, p.x);
  ASSERT_DOUBLE_EQ(4, p.y);
}
