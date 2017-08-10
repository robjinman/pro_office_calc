#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/player.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_components.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/audio_manager.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/animation_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/inventory_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/damage_system.hpp"


//===========================================
// Player::Player
//===========================================
Player::Player(EntityManager& entityManager, AudioManager& audioManager, double tallness,
  std::unique_ptr<Camera> camera)
  : m_entityManager(entityManager),
    m_audioManager(audioManager),
    m_camera(std::move(camera)),
    m_tallness(tallness) {}

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
  return m_camera->height - m_tallness;
}

//===========================================
// Player::headHeight
//===========================================
double Player::headHeight() const {
  return m_camera->height;
}

//===========================================
// Player::getTallness
//===========================================
double Player::getTallness() const {
  return m_tallness;
}

//===========================================
// Player::changeTallness
//===========================================
void Player::changeTallness(double delta) {
  m_tallness += delta;
  m_camera->height += delta;
}

//===========================================
// Player::setFeetHeight
//===========================================
void Player::setFeetHeight(double h) {
  m_camera->height = h + m_tallness;
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
  if (feetHeight() - zone.floorHeight + deltaH < 0) {
    // Only permit positive delta
    if (deltaH <= 0) {
      setFeetHeight(zone.floorHeight);
      return;
    }
  }
  // If applying this delta puts the player's head through the ceiling
  else if (zone.ceilingHeight - headHeight() + deltaH < 0) {
    // Only permit negative delta
    if (deltaH >= 0) {
      return;
    }
  }
  m_camera->height += deltaH;
}

//===========================================
// Player::pos
//===========================================
const Point& Player::pos() const {
  return m_camera->pos;
}

//===========================================
// Player::setPosition
//===========================================
void Player::setPosition(entityId_t zoneId, const Point& pos) {
  m_camera->pos = pos;
  currentRegion = zoneId;
}

//===========================================
// Player::hRotate
//===========================================
void Player::hRotate(double da) {
  m_camera->angle += da;
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
  InventorySystem& inventorySystem = m_entityManager
    .system<InventorySystem>(ComponentKind::C_INVENTORY);

  if (inventorySystem.getBucketValue("ammo") > 0) {
    AnimationSystem& animationSystem = m_entityManager
      .system<AnimationSystem>(ComponentKind::C_ANIMATION);

    DamageSystem& damageSystem = m_entityManager
      .system<DamageSystem>(ComponentKind::C_DAMAGE);

    animationSystem.playAnimation(sprite, "shoot", false);
    m_audioManager.playSound("pistol_shoot");
    inventorySystem.subtractFromBucket("ammo", 1);
    damageSystem.damageAtIntersection(0, 0, 1);
  }
}

//===========================================
// Player::camera
//===========================================
const Camera& Player::camera() const {
  return *m_camera;
}
