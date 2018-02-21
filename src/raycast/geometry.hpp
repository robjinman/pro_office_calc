#ifndef __PROCALC_RAYCAST_GEOMETRY_HPP__
#define __PROCALC_RAYCAST_GEOMETRY_HPP__


#include <cmath>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include <limits>


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

inline double dotProduct(const Vec2f& A, const Vec2f& B) {
  return A.x * B.x + A.y * B.y;
}

inline double angle(const Vec2f& A, const Vec2f& B) {
    return acos(dotProduct(A, B) / (length(A) * length(B)));
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

inline double clipNumber(double x, const Range& range) {
  if (x < range.a) {
    x = range.a;
  }
  else if (x > range.b) {
    x = range.b;
  }
  return x;
}

struct Circle {
  Point pos;
  double radius;
};

Point lineIntersect(const Line& l0, const Line& l1);
bool isBetween(double x, double a, double b, double delta = 0.00001);
bool lineSegmentIntersect(const LineSegment& l0, const LineSegment& l1, Point& p);
bool lineSegmentCircleIntersect(const Circle& circle, const LineSegment& lseg);
double distanceFromLine(const Line& l, const Point& p);
Point projectionOntoLine(const Line& l, const Point& p);
Point clipToLineSegment(const Point& p, const LineSegment& lseg);
LineSegment transform(const LineSegment& lseg, const Matrix& m);
Vec2f normalise(const Vec2f& v);
double normaliseAngle(double a);

enum clipResult_t { CLIPPED_TO_TOP, CLIPPED_TO_BOTTOM, NOT_CLIPPED };
clipResult_t clipNumber(double x, const Range& range, double& result);

#ifdef DEBUG
#include <ostream>
std::ostream& operator<<(std::ostream& os, const Point& lseg);
std::ostream& operator<<(std::ostream& os, const LineSegment& lseg);
std::ostream& operator<<(std::ostream& os, const Line& line);
std::ostream& operator<<(std::ostream& os, const Circle& circ);
std::ostream& operator<<(std::ostream& os, const Matrix& mat);
#endif


#endif
