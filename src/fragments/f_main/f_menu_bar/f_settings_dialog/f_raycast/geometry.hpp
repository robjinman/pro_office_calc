#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_GEOMETRY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_GEOMETRY_HPP__


#include <cmath>
#include <array>
#include <string>
#include <memory>


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

Point operator+(const Point& lhs, const Point& rhs);
Point operator-(const Point& lhs, const Point& rhs);
Point operator/(const Point& lhs, double rhs);

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

  //LineSegment& transform(const Matrix& m);

  Point A;
  Point B;
};

typedef std::unique_ptr<LineSegment> pLineSegment_t;

Point lineIntersect(const Line& l0, const Line& l1);
bool isBetween(double x, double a, double b);
bool lineSegmentIntersect(const LineSegment& l0, const LineSegment& l1, Point& p);
LineSegment transform(const LineSegment& lseg, const Matrix& m);
double distance(const Point& A, const Point& B);

#ifdef DEBUG
#include <ostream>
std::ostream& operator<<(std::ostream& os, const Point& lseg);
std::ostream& operator<<(std::ostream& os, const LineSegment& lseg);
std::ostream& operator<<(std::ostream& os, const Matrix& mat);
#endif


#endif
