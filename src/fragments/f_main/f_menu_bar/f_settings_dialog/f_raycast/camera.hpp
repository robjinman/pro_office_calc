#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_CAMERA_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_CAMERA_HPP__


#include "utils.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"


struct Camera {
  Vec2f pos;
  double angle;
  double hFov = DEG_TO_RAD(60.0);
  double vFov = DEG_TO_RAD(50.0);

  void setTransform(const Matrix& m) {
    pos.x = m.tx;
    pos.y = m.ty;
    angle = m.a();
  }

  Matrix matrix() const {
    return Matrix(angle, Vec2f(pos.x, pos.y));
  }
};


#endif
