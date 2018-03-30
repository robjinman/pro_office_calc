#ifndef __PROCALC_RAYCAST_CAMERA_HPP__
#define __PROCALC_RAYCAST_CAMERA_HPP__


#include "utils.hpp"
#include "raycast/geometry.hpp"
#include "raycast/component.hpp"
#include "raycast/spatial_components.hpp"


class Camera {
  public:
    Camera(double vpW, double hFov, double vFov, CVRect& body, double height)
      : hFov(hFov),
        vFov(vFov),
        F(vpW / (2.0 * tan(0.5 * hFov))),
        height(height),
        m_body(body) {}

    double vAngle = 0.001;
    double hFov = DEG_TO_RAD(60.0);
    double vFov = DEG_TO_RAD(50.0);

    const double F;
    double height;

    const CZone& zone() const {
      return *m_body.zone;
    }

    entityId_t zoneId() const {
      return zone().entityId();
    }

    void setTransform(const Matrix& m) {
      m_body.pos.x = m.tx();
      m_body.pos.y = m.ty();
      m_body.angle = m.a();
    }

    Matrix matrix() const {
      return Matrix(m_body.angle, Vec2f(m_body.pos.x, m_body.pos.y));
    }

    double angle() const {
      return m_body.angle;
    }

    const Point& pos() const {
      return m_body.pos;
    }

  private:
    CVRect& m_body;
};


#endif
