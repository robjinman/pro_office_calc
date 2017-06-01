#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_GEOMETRY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_GEOMETRY_HPP__


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

struct Matrix {
  Matrix()
    : data{{
        {{1.0, 0.0, 0.0}},
        {{0.0, 1.0, 0.0}},
        {{0.0, 0.0, 1.0}}
      }},
      tx(data[0][2]),
      ty(data[1][2]) {}

  std::array<std::array<double, 3>, 3> data;
  double& tx;
  double& ty;

  std::array<double, 3>& operator[](int idx) {
    return data[idx];
  }

  const std::array<double, 3>& operator[](int idx) const {
    return data[idx];
  }

  Matrix& operator=(const Matrix& rhs) {
    data = rhs.data;
  }
};

struct Primitive {
  virtual ~Primitive() {}
};

struct LineSegment : public Primitive {
  LineSegment()
    : A(), B() {}

  LineSegment(const Point& A, const Point& B)
    : A(A), B(B) {}

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
