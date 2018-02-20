#ifndef __PROCALC_RAYCAST_CAMERA_HPP__
#define __PROCALC_RAYCAST_CAMERA_HPP__


#include "utils.hpp"
#include "raycast/geometry.hpp"
#include "raycast/component.hpp"


class CVRect;
class SpatialSystem;

class Camera {
  public:
    Camera(double vpW, double hFov, double vFov, entityId_t body, double height,
      SpatialSystem& spatialSystem);

    double vAngle = 0.001;
    double hFov = DEG_TO_RAD(60.0);
    double vFov = DEG_TO_RAD(50.0);

    const double F;

    entityId_t body;
    double height;

    void setTransform(const Matrix& m);
    Matrix matrix() const;

    double angle() const;
    const Point& pos() const;

  private:
    SpatialSystem& m_spatialSystem;

    CVRect& getBody() const;
};


#endif
