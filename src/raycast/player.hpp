#ifndef __PROCALC_RAYCAST_PLAYER_HPP__
#define __PROCALC_RAYCAST_PLAYER_HPP__


#include <memory>
#include "raycast/component.hpp"
#include "raycast/timing.hpp"
#include "raycast/geometry.hpp"
#include "raycast/camera.hpp"
#include "raycast/system_accessor.hpp"


const double PLAYER_STEP_HEIGHT = 16.0;
const double PLAYER_MAX_PITCH = 20.0;


class CZone;
class CVRect;
class EntityManager;
class AudioService;
namespace parser { struct Object; }
class TimeService;
struct EBucketItemsChange;

class Player : private SystemAccessor {
  public:
    Player(EntityManager& entityManager, AudioService& audioService, TimeService& timeService,
      const parser::Object& obj, entityId_t parentId, const Matrix& parentTransform);

    bool alive = true;
    double vVelocity = 0;
    double activationRadius = 80.0;
    double collectionRadius = 50.0;
    bool invincible = false;

    entityId_t crosshair = -1;
    entityId_t sprite = -1;
    entityId_t red = -1;
    entityId_t body;

    entityId_t region() const;

    bool aboveGround() const;
    bool belowGround() const;

    double feetHeight() const;
    double headHeight() const;
    double eyeHeight() const;

    double getTallness() const;
    void changeTallness(double delta);
    void setFeetHeight(double h);
    void setEyeHeight(double h);

    void changeHeight(const CZone& zone, double deltaH);

    const Point& pos() const;
    void setPosition(const Point& pos);

    Vec2f dir() const;

    void hRotate(double da);
    void vRotate(double da);

    void shoot();
    void jump();

    const Camera& camera() const;

  private:
    void constructPlayer(const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);

    void constructInventory();

    void setupHudShowHide();
    void setupBody(CZone& zone, const Matrix& m, double tallness);
    void setupBodyAnimations();
    void setupCrosshair();
    void setupGunSprite();
    void setupPlayerBehaviour();
    void setupItemsDisplay();
    void setupAmmoCounter();
    void setupHealthCounter();
    void setupHudBackground();

    void updateItemsDisplay(double itemsDisplayW_px, const EBucketItemsChange& e) const;
    void showHud();
    void hideHud();

    void makeTween(const std::string& name, double duration, entityId_t overlayId, double fromY,
      double toY);

    CVRect& getBody() const;

    EntityManager& m_entityManager;
    AudioService& m_audioService;
    TimeService& m_timeService;
    std::unique_ptr<Camera> m_camera;
    Debouncer m_shootTimer;

    entityId_t m_ammoId;
    entityId_t m_healthId;
    entityId_t m_itemsId;
    entityId_t m_hudBgId;
};


#endif
