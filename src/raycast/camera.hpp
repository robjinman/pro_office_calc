#ifndef __PROCALC_RAYCAST_CAMERA_HPP__
#define __PROCALC_RAYCAST_CAMERA_HPP__


#include "utils.hpp"
#include "raycast/geometry.hpp"
#include "raycast/spatial_components.hpp"


struct Camera {
  Camera(double vpW, double hFov, double vFov, CVRect& body)
    : body(body),
      hFov(hFov),
      vFov(vFov),
      F(vpW / (2.0 * tan(0.5 * hFov))) {}

  CVRect& body;
  double height;

  double vAngle = 0.001;
  double hFov = DEG_TO_RAD(60.0);
  double vFov = DEG_TO_RAD(50.0);

  const double F;

  void setTransform(const Matrix& m) {
    body.pos.x = m.tx();
    body.pos.y = m.ty();
    body.angle = m.a();
  }

  Matrix matrix() const {
    return Matrix(body.angle, Vec2f(body.pos.x, body.pos.y));
  }
};


#endif
