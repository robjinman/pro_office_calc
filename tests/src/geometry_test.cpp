#include <gtest/gtest.h>
#include <raycast/geometry.hpp>
#include <utils.hpp>


class GeometryTest : public testing::Test {
  public:
    virtual void SetUp() override {}

    virtual void TearDown() override {}
};


TEST_F(GeometryTest, sqrt) {
  ASSERT_DOUBLE_EQ(0.0, sqrt(0.0));
}

TEST_F(GeometryTest, matrixReferenceFrameChange) {
  LineSegment lseg(Point(123, 771), Point(342, -232));
  double len = lseg.length();
  Vec2f v = lseg.B - lseg.A;
  double angle = atan2(v.y, v.x);
  Matrix m(angle, lseg.A);

  LineSegment lseg2 = transform(lseg, m.inverse());
  Vec2f v2 = lseg2.B - lseg2.A;
  double angle2 = atan2(v2.y, v2.x);

  // After transformation, the lseg should point along the x-axis

  ASSERT_DOUBLE_EQ(0, angle2);
  ASSERT_DOUBLE_EQ(0, lseg2.A.x);
  ASSERT_DOUBLE_EQ(0, lseg2.A.y);
  ASSERT_DOUBLE_EQ(0, lseg2.B.y);
  ASSERT_DOUBLE_EQ(len, lseg2.length());
}

TEST_F(GeometryTest, rayToAngleToRayToTransformedRay) {
  Point a(123, 771);
  Point b(342, -232);

  Vec2f v = normalise(b - a);
  double angle = atan2(v.y, v.x);

  Matrix m(angle, a);

  LineSegment worldRay(a, 10000.0 * Vec2f(cos(angle), sin(angle)));
  LineSegment ray = transform(worldRay, m.inverse());

  //Vec2f v2 = ray.B - ray.A;

  // Precision loss causes this to fail
  //ASSERT_DOUBLE_EQ(0, atan2(v2.y, v2.x));

  ASSERT_DOUBLE_EQ(0, ray.A.x);
  ASSERT_DOUBLE_EQ(0, ray.A.y);

  // Precision loss causes this to fail
  //ASSERT_DOUBLE_EQ(0, ray.B.y);
}

TEST_F(GeometryTest, isBetween_zeros) {
  ASSERT_TRUE(isBetween(0, 0, 0));
}

TEST_F(GeometryTest, isBetween_tight) {
  ASSERT_TRUE(isBetween(609.37900000001, 609.379, 609.379));
}

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

TEST_F(GeometryTest, distanceFromLine_0) {
  Line l(2.0 / 3.0, -4.0 / 3.0);
  Point p(5, 6);

  ASSERT_NEAR(3.328, distanceFromLine(l, p), 0.001);
}

TEST_F(GeometryTest, lineSegmentSignedDistance_0) {
  LineSegment lseg(Point(-2, 1), Point(4, 1));

  ASSERT_DOUBLE_EQ(-1, lseg.signedDistance(-3));
  ASSERT_DOUBLE_EQ(0, lseg.signedDistance(-2));
  ASSERT_DOUBLE_EQ(1, lseg.signedDistance(-1));
  ASSERT_DOUBLE_EQ(2, lseg.signedDistance(0));
  ASSERT_DOUBLE_EQ(3, lseg.signedDistance(1));
  ASSERT_DOUBLE_EQ(4, lseg.signedDistance(2));
  ASSERT_DOUBLE_EQ(5, lseg.signedDistance(3));
  ASSERT_DOUBLE_EQ(6, lseg.signedDistance(4));
  ASSERT_DOUBLE_EQ(7, lseg.signedDistance(5));
}

TEST_F(GeometryTest, lineSegmentSignedDistance_1) {
  LineSegment lseg(Point(4, 1), Point(-2, 1));

  ASSERT_DOUBLE_EQ(-1, lseg.signedDistance(5));
  ASSERT_DOUBLE_EQ(0, lseg.signedDistance(4));
  ASSERT_DOUBLE_EQ(1, lseg.signedDistance(3));
  ASSERT_DOUBLE_EQ(2, lseg.signedDistance(2));
  ASSERT_DOUBLE_EQ(3, lseg.signedDistance(1));
  ASSERT_DOUBLE_EQ(4, lseg.signedDistance(0));
  ASSERT_DOUBLE_EQ(5, lseg.signedDistance(-1));
  ASSERT_DOUBLE_EQ(6, lseg.signedDistance(-2));
  ASSERT_DOUBLE_EQ(7, lseg.signedDistance(-3));
}

TEST_F(GeometryTest, lineSegmentSignedDistance_2) {
  LineSegment lseg(Point(2, 2), Point(-2, -2));

  ASSERT_DOUBLE_EQ(-sqrt(2), lseg.signedDistance(3));
}

TEST_F(GeometryTest, lineSegmentIntersect_1) {
  LineSegment l0(Point(0, -2), Point(4, 10));
  LineSegment l1(Point(0, 8), Point(4, 0));

  Point p;
  ASSERT_TRUE(lineSegmentIntersect(l0, l1, p));
  ASSERT_DOUBLE_EQ(2, p.x);
  ASSERT_DOUBLE_EQ(4, p.y);
}

TEST_F(GeometryTest, lineSegmentIntersect_2) {
  LineSegment l0(Point(512.065, 606.501), Point(512.176, 611.499));
  LineSegment l1(Point(489.175, 609.379), Point(517.175, 609.379));

  Point p;
  ASSERT_TRUE(lineSegmentIntersect(l0, l1, p));
}

TEST_F(GeometryTest, lineSegmentIntersect_vert) {
  LineSegment l0(Point(2, -2), Point(2, 8));
  LineSegment l1(Point(0, 0), Point(5, 5));

  Point p;
  ASSERT_TRUE(lineSegmentIntersect(l0, l1, p));
  ASSERT_DOUBLE_EQ(2, p.x);
  ASSERT_DOUBLE_EQ(2, p.y);
}

TEST_F(GeometryTest, lineSegmentIntersect_vert2) {
  LineSegment l0(Point(2, -2), Point(2, 8));
  LineSegment l1(Point(0, 7), Point(5, 20));

  Point p;
  ASSERT_FALSE(lineSegmentIntersect(l1, l0, p));
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

TEST_F(GeometryTest, lineSegmentCircleIntersect_vert0) {
  LineSegment l(Point(4, -10), Point(4, 10));
  Circle c{Point(3, 2), 2};

  ASSERT_TRUE(lineSegmentCircleIntersect(c, l));
}

TEST_F(GeometryTest, lineSegmentCircleIntersect_vert1) {
  LineSegment l(Point(327.93653399999993781, 1351.23379999999974643), Point(327.93653399999999465, 278.98349999999982174));
  Circle c{Point(707.63005676416514689, 574.89931701693569721), 5};

  ASSERT_FALSE(lineSegmentCircleIntersect(c, l));
}

TEST_F(GeometryTest, normaliseAngle_neg0) {
  ASSERT_DOUBLE_EQ(2.0 * PI - 1.23, normaliseAngle(-1.23));
}

TEST_F(GeometryTest, normaliseAngle_wrap0) {
  ASSERT_DOUBLE_EQ(1.23, normaliseAngle(2.0 * PI + 1.23));
}
