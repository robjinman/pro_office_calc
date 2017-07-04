#include <algorithm>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "utils.hpp"


#ifdef DEBUG
using std::ostream;

ostream& operator<<(ostream& os, const Point& pt) {
  os << "(" << pt.x << ", " << pt.y << ")";
  return os;
}

ostream& operator<<(ostream& os, const LineSegment& lseg) {
  os << "LineSegment " << lseg.A << ", " << lseg.B;
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
// transform
//===========================================
LineSegment transform(const LineSegment& lseg, const Matrix& m) {
  return LineSegment(m * lseg.A, m * lseg.B);
}

//===========================================
// lineIntersect
//===========================================
Point lineIntersect(const Line& l0, const Line& l1) {
  Point p;

  p.x = (l1.c - l0.c) / (l0.m - l1.m);
  p.y = l0.m * p.x + l0.c;

  return p;
}

//===========================================
// isBetween
//===========================================
bool isBetween(double x, double a, double b) {
  if (a < b) {
    return x >= a && x <= b;
  }
  return x >= b && x <= a;
}

//===========================================
// lineSegmentIntersect
//===========================================
bool lineSegmentIntersect(const LineSegment& l0, const LineSegment& l1, Point& p) {
  p = lineIntersect(l0.line(), l1.line());
  return isBetween(p.x, l0.A.x, l0.B.x) && isBetween(p.x, l1.A.x, l1.B.x);
}

//===========================================
// lineSegmentCircleIntersect
//===========================================
bool lineSegmentCircleIntersect(const Circle& circ, const LineSegment& lseg) {
  if (distance(lseg.A, circ.pos) <= circ.radius || distance(lseg.B, circ.pos) <= circ.radius) {
    return true;
  }

  Line l = lseg.line();

  double a = pow(l.m, 2) + 1.0;
  double b = 2.0 * l.m * l.c - 2.0 * l.m * circ.pos.y - 2.0 * circ.pos.x;
  double c = pow(circ.pos.x, 2) + pow(circ.pos.y, 2) + pow(l.c, 2) - 2.0 * circ.pos.y * l.c
    - pow(circ.radius, 2);

  double discriminant = b * b - 4.0 * a * c;
  if (discriminant < 0.0) {
    return false;
  }

  double x0 = (-b + sqrt(discriminant)) / (2.0 * a);
  double y0 = l.m * x0 + l.c;

  double x1 = (-b - sqrt(discriminant)) / (2.0 * a);
  double y1 = l.m * x1 + l.c;

  return (isBetween(x0, lseg.A.x, lseg.B.x) && isBetween(y0, lseg.A.y, lseg.B.y))
    || (isBetween(x1, lseg.A.x, lseg.B.x) && isBetween(y1, lseg.A.y, lseg.B.y));
}

//===========================================
// distanceFromLine
//===========================================
double distanceFromLine(const Line& l, const Point& p) {
  return (l.m * p.x - p.y + l.c) / sqrt(l.m * l.m + 1);
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
// angleBetween
//===========================================
double angleBetween(const Line& l0, const Line& l1) {
  return atan(l0.m) - atan(l1.m);
}

//===========================================
// clipNumber
//===========================================
double clipNumber(double x, const Size& range) {
  if (x < range.x) {
    x = range.x;
  }
  else if (x > range.y) {
    x = range.y;
  }
  return x;
}

//===========================================
// clipNumber
//===========================================
clipResult_t clipNumber(double x, const Size& range, double& result) {
  if (x < range.x) {
    result = range.x;
    return CLIPPED_TO_BOTTOM;
  }
  else if (x > range.y) {
    result = range.y;
    return CLIPPED_TO_TOP;
  }
  else {
    result = x;
    return NOT_CLIPPED;
  }
}
