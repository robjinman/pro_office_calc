#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_GEOMETRY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_GEOMETRY_HPP__


#include <string>


struct Point {
  Point()
    : x(0.0), y(0.0) {}

  Point(double x, double y)
    : x(x), y(y) {}

  double x;
  double y;
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
#endif


#endif
