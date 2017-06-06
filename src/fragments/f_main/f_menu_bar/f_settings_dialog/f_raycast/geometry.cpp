#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"


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
    {{A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][1] * B[2][0],
      A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][1] * B[2][1],
      A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][1] * B[2][2]}},
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
    }},
    tx(data[0][2]),
    ty(data[1][2]) {}

//===========================================
// Matrix::Matrix
//===========================================
Matrix::Matrix(double a, Vec2f t)
  : data{{
      {{cos(a), -sin(a), t.x}},
      {{sin(a), cos(a), t.y}},
      {{0.0, 0.0, 1.0}}
    }},
    tx(data[0][2]),
    ty(data[1][2]) {}

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
// distance
//===========================================
double distance(const Point& A, const Point& B) {
  return sqrt((B.x - A.x) * (B.x - A.x) + (B.y - A.y) * (B.y - A.y));
}
