#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_GEOMETRY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_GEOMETRY_HPP__


#include <cmath>
#include <array>
#include <string>


struct Point {
  Point()
    : x(0.0), y(0.0) {}

  Point(double x, double y)
    : x(x), y(y) {}

  double x;
  double y;
};

typedef Point Vec2f;

struct Matrix {
  Matrix()
    : data{{
        {{1.0, 0.0, 0.0}},
        {{0.0, 1.0, 0.0}},
        {{0.0, 0.0, 1.0}}
      }},
      tx(data[0][2]),
      ty(data[1][2]) {}

  Matrix(double a, Vec2f t)
    : data{{
        {{cos(a), -sin(a), t.x}},
        {{sin(a), cos(a), t.y}},
        {{0.0, 0.0, 1.0}}
      }},
      tx(data[0][2]),
      ty(data[1][2]) {}

  std::array<std::array<double, 3>, 3> data;
  double& tx;
  double& ty;

  double a() const {
    return acos(data[0][0]);
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
    : A(), B() {}

  LineSegment(const Point& A, const Point& B)
    : A(A), B(B) {}

  Line line() const {
    Line l;
    l.m = (B.y - A.y) / (B.x - A.x);
    l.c = A.y - l.m * A.x;

    return l;
  }

  Point A;
  Point B;
};

#ifdef DEBUG
#include <ostream>
std::ostream& operator<<(std::ostream& os, const Point& lseg);
std::ostream& operator<<(std::ostream& os, const LineSegment& lseg);
std::ostream& operator<<(std::ostream& os, const Matrix& mat);
#endif


#endif
