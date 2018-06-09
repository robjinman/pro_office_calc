#include "raycast/player.hpp"
#include "raycast/camera.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/audio_service.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/damage_system.hpp"


const double FOREHEAD_SIZE = 15.0;
const double COLLISION_RADIUS = 10.0;


//===========================================
// Player::Player
//===========================================
Player::Player(EntityManager& entityManager, AudioService& audioService, double tallness,
  CZone& zone, const Matrix& transform)
  : m_entityManager(entityManager),
    m_audioService(audioService),
    m_shootTimer(0.5) {

  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  body = Component::getIdFromString("player");

  CVRect* b = new CVRect(body, zone.entityId(), Size(0, 0));
  b->setTransform(transform);
  b->zone = &zone;
  b->size.x = 60.0;
  b->size.y = tallness + FOREHEAD_SIZE;

  spatialSystem.addComponent(pCSpatial_t(b));

  m_camera.reset(new Camera(renderSystem.rg.viewport.x, DEG_TO_RAD(60), DEG_TO_RAD(50), *b,
    tallness - FOREHEAD_SIZE + zone.floorHeight));
}

//===========================================
// Player::region
//===========================================
entityId_t Player::region() const {
  return m_entityManager.getComponent<CVRect>(body, ComponentKind::C_SPATIAL).zone->entityId();
}

//===========================================
// Player::aboveGround
//===========================================
bool Player::aboveGround(const CZone& zone) const {
  return feetHeight() - 0.1 > zone.floorHeight;
}

//===========================================
// Player::belowGround
//===========================================
bool Player::belowGround(const CZone& zone) const {
  return feetHeight() + 0.1 < zone.floorHeight;
}

//===========================================
// Player::feetHeight
//===========================================
double Player::feetHeight() const {
  return eyeHeight() + FOREHEAD_SIZE - getTallness();
}

//===========================================
// Player::headHeight
//===========================================
double Player::headHeight() const {
  return eyeHeight() + FOREHEAD_SIZE;
}

//===========================================
// Player::eyeHeight
//===========================================
double Player::eyeHeight() const {
  return m_camera->height;
}

//===========================================
// Player::getTallness
//===========================================
double Player::getTallness() const {
  return getBody().size.y;
}

//===========================================
// Player::changeTallness
//===========================================
void Player::changeTallness(double delta) {
  getBody().size.y += delta;
  m_camera->height += delta;
}

//===========================================
// Player::setFeetHeight
//===========================================
void Player::setFeetHeight(double h) {
  m_camera->height = h + getTallness() - FOREHEAD_SIZE;
}

//===========================================
// Player::setEyeHeight
//===========================================
void Player::setEyeHeight(double h) {
  m_camera->height = h;
}

//===========================================
// Player::changeHeight
//===========================================
void Player::changeHeight(const CZone& zone, double deltaH) {
  // If applying this delta puts the player's feet through the floor
  if (feetHeight() + deltaH < zone.floorHeight) {
    // Only permit positive delta
    if (deltaH <= 0) {
      // Reset to floor height
      setFeetHeight(zone.floorHeight);
      return;
    }
  }
  // If applying this delta puts the player's head through the ceiling
  else if (zone.hasCeiling && (headHeight() + deltaH > zone.ceilingHeight)) {
    // Only permit negative delta
    if (deltaH >= 0) {
      vVelocity = 0;
      return;
    }
  }
  m_camera->height += deltaH;
}

//===========================================
// Player::pos
//===========================================
const Point& Player::pos() const {
  return getBody().pos;
}

//===========================================
// Player::getBody
//===========================================
CVRect& Player::getBody() const {
  return m_entityManager.getComponent<CVRect>(body, ComponentKind::C_SPATIAL);
}

//===========================================
// Player::setPosition
//===========================================
void Player::setPosition(const Point& pos) {
  getBody().pos = pos;
}

//===========================================
// Player::hRotate
//===========================================
void Player::hRotate(double da) {
  getBody().angle += da;
}

//===========================================
// Player::vRotate
//===========================================
void Player::vRotate(double da) {
  if (fabs(m_camera->vAngle + da) <= DEG_TO_RAD(20)) {
    m_camera->vAngle += da;
  }
}

//===========================================
// Player::shoot
//===========================================
void Player::shoot() {
  if (m_shootTimer.ready()) {
    InventorySystem& inventorySystem = m_entityManager
      .system<InventorySystem>(ComponentKind::C_INVENTORY);
    AnimationSystem& animationSystem = m_entityManager
      .system<AnimationSystem>(ComponentKind::C_ANIMATION);

    if (inventorySystem.getBucketValue(body, "ammo") > 0) {
      DamageSystem& damageSystem = m_entityManager
        .system<DamageSystem>(ComponentKind::C_DAMAGE);

      animationSystem.playAnimation(sprite, "shoot", false);
      animationSystem.playAnimation(body, "shoot", false);
      m_audioService.playSound("pistol_shoot");
      inventorySystem.subtractFromBucket(body, "ammo", 1);
      damageSystem.damageAtIntersection(Vec2f(1, 0), 0, 1);
    }
    else {
      animationSystem.playAnimation(sprite, "shoot_no_ammo", false);
      m_audioService.playSound("click");
    }
  }
}

//===========================================
// Player::camera
//===========================================
const Camera& Player::camera() const {
  return *m_camera;
}
