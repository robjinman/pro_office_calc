#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_PLAYER_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_PLAYER_HPP__


#include <memory>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"


const double PLAYER_STEP_HEIGHT = 16.0;


class CZone;
class CVRect;
class Camera;
class EntityManager;
class AudioManager;

class Player {
  public:
    Player(EntityManager& entityManager, AudioManager& audioManager, double tallness,
      std::unique_ptr<Camera> camera, CVRect& body);

    double vVelocity = 0;
    double activationRadius = 100.0;
    double collectionRadius = 50.0;
    entityId_t currentRegion = -1;

    entityId_t crosshair;
    entityId_t sprite;
    CVRect& body;

    bool aboveGround(const CZone& zone) const;
    bool belowGround(const CZone& zone) const;

    double feetHeight() const;
    double headHeight() const;

    double getTallness() const;
    void changeTallness(double delta);
    void setFeetHeight(double h);
    void setEyeHeight(double h);

    void changeHeight(const CZone& zone, double deltaH);

    const Point& pos() const;
    void setPosition(entityId_t zoneId, const Point& pos);

    void hRotate(double da);
    void vRotate(double da);

    void shoot();

    const Camera& camera() const;

  private:
    EntityManager& m_entityManager;
    AudioManager& m_audioManager;
    std::unique_ptr<Camera> m_camera;
    double m_tallness = 50;
};


#endif
