#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_PLAYER_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_PLAYER_HPP__


#include <memory>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"


const double PLAYER_STEP_HEIGHT = 16.0;
// World units per second
const double PLAYER_FALL_SPEED = 250.0;


class Player {
  public:
    Player(double tallness, std::unique_ptr<Camera> camera)
      : m_camera(std::move(camera)),
        m_tallness(tallness) {}

    bool heavy = true;
    double airTime = 0.1;

    bool aboveGround(const Region& region) const {
      return feetHeight() - 0.1 > region.floorHeight;
    }

    bool belowGround(const Region& region) const {
      return feetHeight() + 0.1 < region.floorHeight;
    }

    double feetHeight() const {
      return m_camera->height - m_tallness;
    }

    double headHeight() const {
      return m_camera->height;
    }

    void changeTallness(double delta) {
      m_tallness += delta;
      m_camera->height += delta;
    }

    void setFeetHeight(double h) {
      m_camera->height = h + m_tallness;
    }

    void setEyeHeight(double h) {
      m_camera->height = h;
    }

    void changeHeight(const Region& region, double deltaH) {
      // If applying this delta puts the player's feet through the floor
      if (feetHeight() - region.floorHeight + deltaH < 0) {
        // Only permit positive delta
        if (deltaH <= 0) {
          setFeetHeight(region.floorHeight);
          return;
        }
      }
      // If applying this delta puts the player's head through the ceiling
      else if (region.ceilingHeight - headHeight() + deltaH < 0) {
        // Only permit negative delta
        if (deltaH >= 0) {
          return;
        }
      }
      m_camera->height += deltaH;
    }

    void setPosition(const Point& pos) {
      m_camera->pos = pos;
    }

    void move(const Vec2f& ds) {
      m_camera->pos = m_camera->pos + ds;
    }

    void hRotate(double da) {
      m_camera->angle += da;
    }

    void vRotate(double da) {
      if (fabs(m_camera->vAngle + da) <= DEG_TO_RAD(20)) {
        m_camera->vAngle += da;
      }
    }

    const Camera& camera() const {
      return *m_camera;
    }

  private:
      std::unique_ptr<Camera> m_camera;
      double m_tallness = 50;
};


#endif
