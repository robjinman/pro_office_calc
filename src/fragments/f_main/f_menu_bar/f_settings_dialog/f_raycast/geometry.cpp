#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"


#ifdef DEBUG
std::ostream& operator<<(std::ostream& os, const Point& pt) {
  os << "(" << pt.x << ", " << pt.y << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const LineSegment& lseg) {
  os << "LineSegment " << lseg.A << ", " << lseg.B;
  return os;
}
#endif
