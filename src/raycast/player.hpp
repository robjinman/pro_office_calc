#ifndef __PROCALC_RAYCAST_PLAYER_HPP__
#define __PROCALC_RAYCAST_PLAYER_HPP__


#include <memory>
#include "raycast/camera.hpp"
#include "raycast/component.hpp"
#include "raycast/timing.hpp"


const double PLAYER_STEP_HEIGHT = 16.0;


class CZone;
class CVRect;
class Camera;
class EntityManager;
class AudioService;

class Player {
  public:
    Player(EntityManager& entityManager, AudioService& audioService, double tallness,
      std::unique_ptr<Camera> camera, CVRect& body);

    double vVelocity = 0;
    double activationRadius = 100.0;
    double collectionRadius = 50.0;
    entityId_t currentRegion = -1;

    entityId_t crosshair = -1;
    entityId_t sprite = -1;
    entityId_t red = -1;
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
    AudioService& m_audioService;
    std::unique_ptr<Camera> m_camera;
    double m_tallness = 50;
    Debouncer m_shootTimer;
};


#endif
