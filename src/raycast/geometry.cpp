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
#endif

//===========================================
// operator+
//===========================================
Point operator+(const Point& lhs, const Point& rhs) {
  return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}

//===========================================
// operator-
//===========================================
Point operator-(const Point& lhs, const Point& rhs) {
  return Point(lhs.x - rhs.x, lhs.y - rhs.y);
}

//===========================================
// operator/
//===========================================
Point operator/(const Point& lhs, double rhs) {
  return Point(lhs.x / rhs, lhs.y / rhs);
}

//===========================================
// operator*
//===========================================
Point operator*(const Point& lhs, double rhs) {
  return Point(lhs.x * rhs, lhs.y * rhs);
}

//===========================================
// operator*
//===========================================
Point operator*(const Matrix& lhs, const Point& rhs) {
  Point p;
  p.x = lhs[0][0] * rhs.x + lhs[0][1] * rhs.y + lhs[0][2];
  p.y = lhs[1][0] * rhs.x + lhs[1][1] * rhs.y + lhs[1][2];
  return p;
}

//===========================================
// operator*
//===========================================
Vec3f operator*(const Matrix& lhs, const Vec3f& rhs) {
  Vec3f p;
  p.x = lhs[0][0] * rhs.x + lhs[0][1] * rhs.y + lhs[0][2];
  p.y = lhs[1][0] * rhs.x + lhs[1][1] * rhs.y + lhs[1][2];
  p.y = lhs[2][0] * rhs.x + lhs[2][1] * rhs.y + lhs[2][2];
  return p;
}

//===========================================
// operator*
//===========================================
Vec3f operator*(const Vec3f& lhs, const Matrix& rhs) {
  Vec3f p;
  p.x = lhs.x * rhs[0][0] + lhs.y * rhs[1][0] + lhs.z * rhs[2][0];
  p.y = lhs.x * rhs[0][1] + lhs.y * rhs[1][1] + lhs.z * rhs[2][1];
  p.z = lhs.x * rhs[0][2] + lhs.y * rhs[1][2] + lhs.z * rhs[2][2];

  return p;
}

//===========================================
// operator*
//===========================================
Matrix operator*(const Matrix& A, const Matrix& B) {
  Matrix m;
  m.data = {{
    {{A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0],
      A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1],
      A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2]}},
    {{A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0],
      A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1],
      A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2]}},
    {{A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0],
      A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1],
      A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2]}}
  }};

  return m;
}

//===========================================
// Matrix::Matrix
//===========================================
Matrix::Matrix()
  : data{{
      {{1.0, 0.0, 0.0}},
      {{0.0, 1.0, 0.0}},
      {{0.0, 0.0, 1.0}}
    }} {}

//===========================================
// Matrix::Matrix
//===========================================
Matrix::Matrix(double a, Vec2f t)
  : data{{
      {{cos(a), -sin(a), t.x}},
      {{sin(a), cos(a), t.y}},
      {{0.0, 0.0, 1.0}}
    }} {}

//===========================================
// Matrix::inverse
//===========================================
Matrix Matrix::inverse() const {
  Matrix m;
  m.data = {{
    {{data[0][0], data[1][0], -data[0][0] * data[0][2] - data[1][0] * data[1][2]}},
    {{data[0][1], data[1][1], -data[0][1] * data[0][2] - data[1][1] * data[1][2]}},
    {{0.0, 0.0, 1.0}}
  }};
  return m;
}

//===========================================
// LineSegment::length
//===========================================
double LineSegment::length() const {
  return distance(A, B);
}

//===========================================
// LineSegment::signedDistance
//===========================================
double LineSegment::signedDistance(const Point& p) const {
  Vec2f AB = B - A;
  Vec2f P = p - A;

  double theta = acos(AB.dot(P) / (::length(P) * ::length(AB)));
  double sign = theta > 0.5 * PI ? -1 : 1;

  return distance(A, p) * sign;
}

//===========================================
// transform
//===========================================
LineSegment transform(const LineSegment& lseg, const Matrix& m) {
  return LineSegment(m * lseg.A, m * lseg.B);
}

//===========================================
// lineIntersect
//===========================================
Point lineIntersect(const Line& l0, const Line& l1, int depth) {
  assert(depth <= 1);

  if (l0.hasSteepGradient() || l1.hasSteepGradient()) {
    Vec3f p;

    Vec3f l0params(l0.a, l0.b, l0.c);
    Vec3f l1params(l1.a, l1.b, l1.c);

    Matrix m(0.25 * PI, Vec2f(0, 0));
    Matrix m_inv = m.inverse();

    Vec3f l0params_ = l0params * m_inv;
    Vec3f l1params_ = l1params * m_inv;

    Line l0_(l0params_.x, l0params_.y, l0params_.z);
    Line l1_(l1params_.x, l1params_.y, l1params_.z);

    if (l0_.hasSteepGradient() || l1_.hasSteepGradient()) {
      return lineIntersect(l0_, l1_, depth + 1);
    }

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
// isBetween
//===========================================
bool isBetween(double x, double a, double b, double delta) {
  if (a < b) {
    return x >= a - delta && x <= b + delta;
  }
  return x >= b - delta && x <= a + delta;
}

//===========================================
// clipToLineSegment
//===========================================
Point clipToLineSegment(const Point& p, const LineSegment& lseg) {
  Point p_ = projectionOntoLine(lseg.line(), p);

  double d = lseg.signedDistance(p_);
  if (d < 0) {
    return lseg.A;
  }
  if (d > lseg.length()) {
    return lseg.B;
  }
  return p_;
}

//===========================================
// lineSegmentIntersect
//===========================================
bool lineSegmentIntersect(const LineSegment& l0, const LineSegment& l1, Point& p) {
  p = lineIntersect(l0.line(), l1.line());
  return (isBetween(p.x, l0.A.x, l0.B.x) && isBetween(p.x, l1.A.x, l1.B.x))
    && (isBetween(p.y, l0.A.y, l0.B.y) && isBetween(p.y, l1.A.y, l1.B.y));
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

//===========================================
// distanceFromLine
//===========================================
double distanceFromLine(const Line& l, const Point& p) {
  return distance(p, projectionOntoLine(l, p));
}

//===========================================
// projectionOntoLine
//===========================================
Point projectionOntoLine(const Line& l, const Point& p) {
  double a = -l.b;
  double b = l.a;
  double c = -p.y * b - p.x * a;
  Line m(a, b, c);
  return lineIntersect(l, m);
}

//===========================================
// normalise
//===========================================
Vec2f normalise(const Vec2f& v) {
  double l = sqrt(v.x * v.x + v.y * v.y);
  return Vec2f(v.x / l, v.y / l);
}

//===========================================
// normaliseAngle
//===========================================
double normaliseAngle(double angle) {
  double q = angle / (2.0 * PI);
  angle = 2.0 * PI * (q - floor(q));

  if (angle < 0.0) {
    return 2.0 * PI + angle;
  }
  else {
    return angle;
  }
}

//===========================================
// clipNumber
//===========================================
clipResult_t clipNumber(double x, const Range& range, double& result) {
  if (x < range.a) {
    result = range.a;
    return CLIPPED_TO_BOTTOM;
  }
  else if (x > range.b) {
    result = range.b;
    return CLIPPED_TO_TOP;
  }
  else {
    result = x;
    return NOT_CLIPPED;
  }
}
