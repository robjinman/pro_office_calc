#ifndef __PROCALC_RAYCAST_PLAYER_HPP__
#define __PROCALC_RAYCAST_PLAYER_HPP__


#include <memory>
#include "raycast/component.hpp"
#include "raycast/timing.hpp"
#include "raycast/geometry.hpp"
#include "raycast/camera.hpp"


const double PLAYER_STEP_HEIGHT = 16.0;
const double PLAYER_MAX_PITCH = 20.0;


class CZone;
class CVRect;
class EntityManager;
class AudioService;
namespace parser { struct Object; }
class TimeService;
class SpatialSystem;
class RenderSystem;
class AnimationSystem;
class DamageSystem;
class BehaviourSystem;
class InventorySystem;
class EventHandlerSystem;

class Player {
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

    const Camera& camera() const;

  private:
    void constructPlayer(const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform, SpatialSystem& spatialSystem, RenderSystem& renderSystem,
      AnimationSystem& animationSystem, DamageSystem& damageSystem,
      BehaviourSystem& behaviourSystem, EventHandlerSystem& eventHandlerSystem);
    void constructInventory(RenderSystem& renderSystem, InventorySystem& inventorySystem,
      EventHandlerSystem& eventHandlerSystem, DamageSystem& damageSystem);
    void setupHudShowHide(RenderSystem& renderSystem, EventHandlerSystem& eventHandlerSystem);

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
