#include <gtest/gtest.h>
#include <fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp>
#include <utils.hpp>


class GeometryTest : public testing::Test {
  public:
    virtual void SetUp() override {}

    virtual void TearDown() override {}
};


TEST_F(GeometryTest, matrix_angle0) {
  double a = DEG_TO_RAD(12.3);

  Matrix m(a, Vec2f(-8, 7));
  ASSERT_DOUBLE_EQ(a, m.a());
}

TEST_F(GeometryTest, matrix_angle1) {
  double a = DEG_TO_RAD(-12.3);

  Matrix m(a, Vec2f(51, 10));
  ASSERT_DOUBLE_EQ(a, m.a());
}

TEST_F(GeometryTest, matrix_angle2) {
  double a = DEG_TO_RAD(-180);

  Matrix m(a, Vec2f(14, 0));
  ASSERT_DOUBLE_EQ(a, m.a());
}

TEST_F(GeometryTest, lineSegmentIntersect) {
  LineSegment l0(Point(0, -2), Point(4, 10));
  LineSegment l1(Point(0, 8), Point(4, 0));

  Point p;
  ASSERT_TRUE(lineSegmentIntersect(l0, l1, p));
  ASSERT_DOUBLE_EQ(2, p.x);
  ASSERT_DOUBLE_EQ(4, p.y);
}

TEST_F(GeometryTest, lineSegmentCircleIntersect_hit0) {
  LineSegment l(Point(0, -2), Point(4, 6));
  Circle c{Point(3, 2), 2};

  ASSERT_TRUE(lineSegmentCircleIntersect(c, l));
}

TEST_F(GeometryTest, lineSegmentCircleIntersect_miss0) {
  LineSegment l(Point(0, -2), Point(4, 6));
  Circle c{Point(-1, 2), 2};

  ASSERT_FALSE(lineSegmentCircleIntersect(c, l));
}

TEST_F(GeometryTest, lineSegmentCircleIntersect_miss1) {
  LineSegment l(Point(-10, 8.9), Point(10, -1.1));
  Circle c{Point(-1, 2), 2};

  ASSERT_FALSE(lineSegmentCircleIntersect(c, l));
}

TEST_F(GeometryTest, lineSegmentCircleIntersect_miss2) {
  LineSegment l(Point(-4, -2), Point(-2, 0));
  Circle c{Point(1, 2), 2};

  ASSERT_FALSE(lineSegmentCircleIntersect(c, l));
}

TEST_F(GeometryTest, normaliseAngle_neg0) {
  ASSERT_DOUBLE_EQ(2.0 * PI - 1.23, normaliseAngle(-1.23));
}

TEST_F(GeometryTest, normaliseAngle_wrap0) {
  ASSERT_DOUBLE_EQ(1.23, normaliseAngle(2.0 * PI + 1.23));
}
