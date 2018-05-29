#include <cassert>
#include <algorithm>
#include "raycast/geometry.hpp"
#include "utils.hpp"


#ifdef DEBUG
using std::ostream;

ostream& operator<<(ostream& os, const Point& pt) {
  os << "(" << pt.x << ", " << pt.y << ")";
  return os;
}

ostream& operator<<(ostream& os, const Vec3f& pt) {
  os << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")";
  return os;
}

ostream& operator<<(ostream& os, const LineSegment& lseg) {
  os << "LineSegment " << lseg.A << ", " << lseg.B;
  return os;
}

ostream& operator<<(ostream& os, const Line& line) {
  os << "Line " << line.a << "x + " << line.b << "y + " << line.c << " = 0";
  return os;
}

ostream& operator<<(ostream& os, const Circle& circ) {
  os.precision(std::numeric_limits<double>::max_digits10);
  os << "Circle " << std::fixed << circ.pos << ", " << circ.radius;
  return os;
}

ostream& operator<<(ostream& os, const Matrix& mat) {
  os << "Matrix " << mat[0][0] << " " << mat[0][1] << " " << mat[0][2] << "\n";
  os << "       " << mat[1][0] << " " << mat[1][1] << " " << mat[1][2] << "\n";
  os << "       " << mat[2][0] << " " << mat[2][1] << " " << mat[2][2];

  return os;
}

ostream& operator<<(ostream& os, const Range& range) {
  os << "Range [" << range.a << ", " << range.b << "]";
  return os;
}
#endif


//===========================================
// lineIntersect
//===========================================
Point lineIntersect(const Line& l0, const Line& l1, int depth) {
  assert(depth <= 1);

  if (l0.hasSteepGradient() || l1.hasSteepGradient()) {
    Vec3f l0params(l0.a, l0.b, l0.c);
    Vec3f l1params(l1.a, l1.b, l1.c);

    static Matrix m(0.25 * PI, Vec2f(0, 0));
    static Matrix m_inv = m.inverse();

    Vec3f l0params_ = l0params * m_inv;
    Vec3f l1params_ = l1params * m_inv;

    Line l0_(l0params_.x, l0params_.y, l0params_.z);
    Line l1_(l1params_.x, l1params_.y, l1params_.z);

    if (l0_.hasSteepGradient() || l1_.hasSteepGradient()) {
      Point p = lineIntersect(l0_, l1_, depth + 1);
      Vec3f p_(p.x, p.y, 0);
      p_ = p_ * m;
      return Point(p_.x, p_.y);
    }

    Vec3f p;
    p.x = (l1_.b * l0_.c - l0_.b * l1_.c) / (l0_.b * l1_.a - l1_.b * l0_.a);
    p.y = (-l0_.a * p.x - l0_.c) / l0_.b;
    p.z = 0;

    p = p * m;

    return Point(p.x, p.y);
  }
  else {
    Point p;
    p.x = (l1.b * l0.c - l0.b * l1.c) / (l0.b * l1.a - l1.b * l0.a);
    p.y = (-l0.a * p.x - l0.c) / l0.b;

    return p;
  }
}

//===========================================
// swapAxes
//===========================================
static Point swapAxes(const Point& pt) {
  return Point(pt.y, pt.x);
}

//===========================================
// lineSegmentCircleIntersect
//===========================================
bool lineSegmentCircleIntersect(const Circle& circ, const LineSegment& lseg) {
  if (distance(lseg.A, circ.pos) <= circ.radius || distance(lseg.B, circ.pos) <= circ.radius) {
    return true;
  }

  Line l = lseg.line();

  const Point& p = circ.pos;
  double r = circ.radius;

  if (l.hasSteepGradient()) {
    return lineSegmentCircleIntersect(Circle{swapAxes(p), r},
      LineSegment(swapAxes(lseg.A), swapAxes(lseg.B)));
  }

  double a = 1.0 + pow(l.a, 2) / pow(l.b, 2);
  double b = 2.0 * (-p.x + (l.a * l.c) / pow(l.b, 2) + (p.y * l.a) / l.b);
  double c = pow(p.x, 2) + pow(l.c, 2) / pow(l.b, 2) + 2.0 * p.y * l.c / l.b + pow(p.y, 2)
    - pow(r, 2);

  double discriminant = b * b - 4.0 * a * c;
  if (discriminant < 0.0) {
    return false;
  }

  double x0 = (-b + sqrt(discriminant)) / (2.0 * a);
  double y0 = -(l.a * x0 + l.c) / l.b;

  double x1 = (-b - sqrt(discriminant)) / (2.0 * a);
  double y1 = -(l.a * x1 + l.c) / l.b;

  return (isBetween(x0, lseg.A.x, lseg.B.x) && isBetween(y0, lseg.A.y, lseg.B.y))
    || (isBetween(x1, lseg.A.x, lseg.B.x) && isBetween(y1, lseg.A.y, lseg.B.y));
}
