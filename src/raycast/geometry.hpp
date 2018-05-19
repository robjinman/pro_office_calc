#ifndef __PROCALC_RAYCAST_GEOMETRY_HPP__
#define __PROCALC_RAYCAST_GEOMETRY_HPP__


#include <cmath>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include <limits>
#include "utils.hpp"


template<class T>
struct Vec2 {
  Vec2()
    : x(0), y(0) {}

  Vec2(T x, T y)
    : x(x), y(y) {}

  T x;
  T y;

  bool operator==(const Vec2<T>& rhs) const {
    return x == rhs.x && y == rhs.y;
  }

  bool operator!=(const Vec2<T>& rhs) const {
    return !(*this == rhs);
  }

  Vec2 operator-() const {
    return Vec2<T>(-x, -y);
  }

  T dot(const Vec2<T>& rhs) const {
    return x * rhs.x + y * rhs.y;
  }
};

typedef Vec2<int> Vec2i;
typedef Vec2<double> Vec2f;
typedef Vec2f Point;
typedef Vec2f Size;

template<class T>
struct Vec3 {
  Vec3()
    : x(0), y(0), z(0) {}

  Vec3(T x, T y, T z)
    : x(x), y(y), z(z) {}

  T x;
  T y;
  T z;

  bool operator==(const Vec3<T>& rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z;
  }

  bool operator!=(const Vec3<T>& rhs) const {
    return !(*this == rhs);
  }

  Vec3 operator-() const {
    return Vec3<T>(-x, -y, -z);
  }

  T dot(const Vec3<T>& rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }
};

typedef Vec3<double> Vec3f;

class Matrix {
  public:
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

inline Matrix operator*(const Matrix& A, const Matrix& B) {
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

inline Matrix::Matrix()
  : data{{
      {{1.0, 0.0, 0.0}},
      {{0.0, 1.0, 0.0}},
      {{0.0, 0.0, 1.0}}
    }} {}

inline Matrix::Matrix(double a, Vec2f t)
  : data{{
      {{cos(a), -sin(a), t.x}},
      {{sin(a), cos(a), t.y}},
      {{0.0, 0.0, 1.0}}
    }} {}

inline Matrix Matrix::inverse() const {
  Matrix m;
  m.data = {{
    {{data[0][0], data[1][0], -data[0][0] * data[0][2] - data[1][0] * data[1][2]}},
    {{data[0][1], data[1][1], -data[0][1] * data[0][2] - data[1][1] * data[1][2]}},
    {{0.0, 0.0, 1.0}}
  }};
  return m;
}

struct Range {
  Range()
    : a(0), b(0) {}

  Range(double a, double b)
    : a(a), b(b) {}

  double a;
  double b;

  bool operator==(const Range& rhs) const {
    return a == rhs.a && b == rhs.b;
  }

  bool operator!=(const Range& rhs) const {
    return !(*this == rhs);
  }
};

inline Point operator+(const Point& lhs, const Point& rhs) {
  return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline Point operator-(const Point& lhs, const Point& rhs) {
  return Point(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline Point operator/(const Point& lhs, double rhs) {
  return Point(lhs.x / rhs, lhs.y / rhs);
}

inline Point operator*(const Point& lhs, double rhs) {
  return Point(lhs.x * rhs, lhs.y * rhs);
}

inline Point operator*(const Matrix& lhs, const Point& rhs) {
  Point p;
  p.x = lhs[0][0] * rhs.x + lhs[0][1] * rhs.y + lhs[0][2];
  p.y = lhs[1][0] * rhs.x + lhs[1][1] * rhs.y + lhs[1][2];
  return p;
}

inline Vec3f operator*(const Matrix& lhs, const Vec3f& rhs) {
  Vec3f p;
  p.x = lhs[0][0] * rhs.x + lhs[0][1] * rhs.y + lhs[0][2];
  p.y = lhs[1][0] * rhs.x + lhs[1][1] * rhs.y + lhs[1][2];
  p.y = lhs[2][0] * rhs.x + lhs[2][1] * rhs.y + lhs[2][2];
  return p;
}

inline Vec3f operator*(const Vec3f& lhs, const Matrix& rhs) {
  Vec3f p;
  p.x = lhs.x * rhs[0][0] + lhs.y * rhs[1][0] + lhs.z * rhs[2][0];
  p.y = lhs.x * rhs[0][1] + lhs.y * rhs[1][1] + lhs.z * rhs[2][1];
  p.z = lhs.x * rhs[0][2] + lhs.y * rhs[1][2] + lhs.z * rhs[2][2];

  return p;
}

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

inline double dotProduct(const Vec2f& A, const Vec2f& B) {
  return A.x * B.x + A.y * B.y;
}

inline double angle(const Vec2f& A, const Vec2f& B) {
    return acos(dotProduct(A, B) / (length(A) * length(B)));
}

struct Line {
  Line(const Point& A, const Point& B) {
    a = B.y - A.y;
    b = A.x - B.x;
    c = -b * A.y - a * A.x;
  }

  Line(double a, double b, double c)
    : a(a), b(b), c(c) {}

  bool hasSteepGradient() const {
    return fabs(a / b) > 999.9;
  }

  double a;
  double b;
  double c;
};

struct LineSegment {
  LineSegment()
    : A(0, 0),
      B(0, 0) {}

  LineSegment(const Point& A_, const Point& B_) {
    A = A_;
    B = B_;
  }

  Line line() const {
    return Line(A, B);
  }

  double length() const;

  double angle() const {
    return atan2(B.y - A.y, B.x - A.x);
  }

  // Signed distance from A. Assumes p lies on line.
  double signedDistance(const Point& p) const;

  Point A;
  Point B;
};

inline double LineSegment::length() const {
  return distance(A, B);
}

inline double LineSegment::signedDistance(const Point& p) const {
  Vec2f AB = B - A;
  Vec2f P = p - A;

  double theta = acos(AB.dot(P) / (::length(P) * ::length(AB)));
  double sign = theta > 0.5 * PI ? -1 : 1;

  return distance(A, p) * sign;
}

struct Circle {
  Point pos;
  double radius;
};

inline bool isBetween(double x, double a, double b, double delta = 0.00001) {
  if (a < b) {
    return x >= a - delta && x <= b + delta;
  }
  return x >= b - delta && x <= a + delta;
}

Point lineIntersect(const Line& l0, const Line& l1, int depth = 0);
bool lineSegmentCircleIntersect(const Circle& circle, const LineSegment& lseg);

inline bool lineSegmentIntersect(const LineSegment& l0, const LineSegment& l1, Point& p) {
  p = lineIntersect(l0.line(), l1.line());
  return (isBetween(p.x, l0.A.x, l0.B.x) && isBetween(p.x, l1.A.x, l1.B.x))
    && (isBetween(p.y, l0.A.y, l0.B.y) && isBetween(p.y, l1.A.y, l1.B.y));
}

inline LineSegment transform(const LineSegment& lseg, const Matrix& m) {
  return LineSegment(m * lseg.A, m * lseg.B);
}

inline Point projectionOntoLine(const Line& l, const Point& p) {
  double a = -l.b;
  double b = l.a;
  double c = -p.y * b - p.x * a;
  Line m(a, b, c);
  return lineIntersect(l, m);
}

inline Point clipToLineSegment(const Point& p, const LineSegment& lseg) {
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

inline double clipNumber(double x, const Range& range) {
  if (x < range.a) {
    x = range.a;
  }
  else if (x > range.b) {
    x = range.b;
  }
  return x;
}

inline double distanceFromLine(const Line& l, const Point& p) {
  return distance(p, projectionOntoLine(l, p));
}

inline Vec2f normalise(const Vec2f& v) {
  double l = sqrt(v.x * v.x + v.y * v.y);
  return Vec2f(v.x / l, v.y / l);
}

inline double normaliseAngle(double angle) {
  static const double pi2 = 2.0 * PI;
  static const double pi2_rp = 1.0 / pi2;

  double n = trunc(angle * pi2_rp);
  angle -= pi2 * n;

  if (angle < 0.0) {
    return pi2 + angle;
  }
  else {
    return angle;
  }
}

enum clipResult_t { CLIPPED_TO_TOP, CLIPPED_TO_BOTTOM, NOT_CLIPPED };

inline clipResult_t clipNumber(double x, const Range& range, double& result) {
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

#ifdef DEBUG
#include <ostream>
std::ostream& operator<<(std::ostream& os, const Point& lseg);
std::ostream& operator<<(std::ostream& os, const LineSegment& lseg);
std::ostream& operator<<(std::ostream& os, const Line& line);
std::ostream& operator<<(std::ostream& os, const Circle& circ);
std::ostream& operator<<(std::ostream& os, const Matrix& mat);
#endif


#endif
