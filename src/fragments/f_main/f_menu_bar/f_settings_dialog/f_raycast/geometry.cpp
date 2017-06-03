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

Point operator*(const Matrix& lhs, const Point& rhs) {
  Point p;
  p.x = lhs[0][0] * rhs.x + lhs[0][1] * rhs.y + lhs[0][2];
  p.y = lhs[1][0] * rhs.x + lhs[1][1] * rhs.y + lhs[1][2];
  return p;
}

Matrix Matrix::inverse() const {
  Matrix m;
  m.data = {{
    {{data[0][0], data[1][0], -data[0][0] * data[0][2] - data[1][0] * data[1][2]}},
    {{data[0][1], data[1][1], -data[0][1] * data[0][2] - data[1][1] * data[1][2]}},
    {{0.0, 0.0, 1.0}}
  }};
  return m;
}
