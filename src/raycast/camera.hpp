#ifndef __PROCALC_RAYCAST_CAMERA_HPP__
#define __PROCALC_RAYCAST_CAMERA_HPP__


#include "utils.hpp"
#include "raycast/geometry.hpp"


struct Camera {
  Camera(double vpW, double hFov, double vFov)
    : hFov(hFov),
      vFov(vFov),
      F(vpW / (2.0 * tan(0.5 * hFov))) {}

  Vec2f pos;
  double angle;
  double height;

  double vAngle = 0.001;
  double hFov = DEG_TO_RAD(60.0);
  double vFov = DEG_TO_RAD(50.0);

  const double F;

  void setTransform(const Matrix& m) {
    pos.x = m.tx();
    pos.y = m.ty();
    angle = m.a();
  }

  Matrix matrix() const {
    return Matrix(angle, Vec2f(pos.x, pos.y));
  }
};


#endif
