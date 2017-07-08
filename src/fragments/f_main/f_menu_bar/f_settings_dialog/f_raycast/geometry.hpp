#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_GEOMETRY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_GEOMETRY_HPP__


#include <cmath>
#include <array>
#include <string>
#include <memory>
#include <vector>


const double SMALL_DOUBLE = 0.001;


struct Point {
  Point()
    : x(0.0), y(0.0) {}

  Point(double x, double y)
    : x(x), y(y) {}

  double x;
  double y;

  bool operator==(const Point& rhs) const {
    return x == rhs.x && y == rhs.y;
  }
};

typedef Point Vec2f;
typedef Vec2f Size;

Point operator+(const Point& lhs, const Point& rhs);
Point operator-(const Point& lhs, const Point& rhs);
Point operator/(const Point& lhs, double rhs);
Point operator*(const Point& lhs, double rhs);

inline bool operator<(const Point& lhs, const Point& rhs) {
  return lhs.x * lhs.x + lhs.y * lhs.y < rhs.x * rhs.x + rhs.y * rhs.y;
}

inline Point operator*(double lhs, const Point& rhs) {
  return rhs * lhs;
}

inline bool pointsEqual(const Point& lhs, const Point& rhs, double delta) {
  return fabs(rhs.x - lhs.x) <= delta && fabs(rhs.y - lhs.y) <= delta;
}

inline double distance(const Point& A, const Point& B) {
  return sqrt((B.x - A.x) * (B.x - A.x) + (B.y - A.y) * (B.y - A.y));
}

inline double length(const Vec2f& v) {
  return sqrt(v.x * v.x + v.y * v.y);
}

struct Matrix {
  Matrix();
  Matrix(double a, Vec2f t);

  std::array<std::array<double, 3>, 3> data;

  double tx() const {
    return data[0][2];
  }

  double ty() const {
    return data[1][2];
  }

  double a() const {
    return atan2(data[1][0], data[1][1]);
  }

  std::array<double, 3>& operator[](int idx) {
    return data[idx];
  }

  const std::array<double, 3>& operator[](int idx) const {
    return data[idx];
  }

  Matrix& operator=(const Matrix& rhs) {
    data = rhs.data;
    return *this;
  }

  Matrix inverse() const;
};

Point operator*(const Matrix& lhs, const Point& rhs);
Matrix operator*(const Matrix& lhs, const Matrix& rhs);

struct Line {
  Line()
    : m(0), c(0) {}

  Line(double m, double c)
    : m(m), c(c) {}

  Point at(double x) const {
    return Point(x, m * x + c);
  }

  double m;
  double c;
};

struct LineSegment {
  LineSegment()
    : A(0, 0),
      B(SMALL_DOUBLE, 0) {}

  LineSegment(const Point& A_, const Point& B_) {
    A = A_;
    B = B_;

    // Avoid vertical lines
    if (fabs(B.x - A.x) < SMALL_DOUBLE) {
      B.x += SMALL_DOUBLE;
    }
  }

  Line line() const {
    Line l;

    l.m = (B.y - A.y) / (B.x - A.x);
    l.c = A.y - l.m * A.x;

    return l;
  }

  double length() const;

  // Signed distance from A
  double signedDistance(double x) const {
    double lineDir = A.x < B.x ? 1.0 : -1.0;
    double pointDir = x > A.x ? 1.0 : -1.0;
    return distance(A, Point(x, line().at(x).y)) * lineDir * pointDir;
  }

  Point A;
  Point B;
};

inline Point clipToLineSegment(const Point& p, const LineSegment& lseg) {
  double d = lseg.signedDistance(p.x);
  if (d < 0) {
    return lseg.A;
  }
  if (d > lseg.length()) {
    return lseg.B;
  }
  return p;
}

struct Circle {
  Point pos;
  double radius;
};

Point lineIntersect(const Line& l0, const Line& l1);
bool isBetween(double x, double a, double b);
bool lineSegmentIntersect(const LineSegment& l0, const LineSegment& l1, Point& p);
bool lineSegmentCircleIntersect(const Circle& circle, const LineSegment& lseg);
double distanceFromLine(const Line& l, const Point& p);
LineSegment transform(const LineSegment& lseg, const Matrix& m);
Vec2f normalise(const Vec2f& v);
double angleBetween(const Line& l0, const Line& l1);
double normaliseAngle(double a);

enum clipResult_t { CLIPPED_TO_TOP, CLIPPED_TO_BOTTOM, NOT_CLIPPED };
double clipNumber(double x, const Size& range);
clipResult_t clipNumber(double x, const Size& range, double& result);

#ifdef DEBUG
#include <ostream>
std::ostream& operator<<(std::ostream& os, const Point& lseg);
std::ostream& operator<<(std::ostream& os, const LineSegment& lseg);
std::ostream& operator<<(std::ostream& os, const Matrix& mat);
#endif


#endif
