#include "raycast/camera.hpp"
#include "raycast/spatial_system.hpp"


//===========================================
// Camera::Camera
//===========================================
Camera::Camera(double vpW, double hFov, double vFov, entityId_t body, double height,
  SpatialSystem& spatialSystem)
  : hFov(hFov),
    vFov(vFov),
    F(vpW / (2.0 * tan(0.5 * hFov))),
    height(height),
    m_spatialSystem(spatialSystem),
    body(body) {}

//===========================================
// Camera::setTransform
//===========================================
void Camera::setTransform(const Matrix& m) {
  CVRect& b = getBody();

  b.pos.x = m.tx();
  b.pos.y = m.ty();
  b.angle = m.a();
}

//===========================================
// Camera::matrix
//===========================================
Matrix Camera::matrix() const {
  CVRect& b = getBody();

  return Matrix(b.angle, Vec2f(b.pos.x, b.pos.y));
}

//===========================================
// Camera::angle
//===========================================
double Camera::angle() const {
  return getBody().angle;
}

//===========================================
// Camera::pos
//===========================================
const Point& Camera::pos() const {
  return getBody().pos;
}

//===========================================
// Camera::getBody
//===========================================
CVRect& Camera::getBody() const {
  return dynamic_cast<CVRect&>(m_spatialSystem.getComponent(body));
}
