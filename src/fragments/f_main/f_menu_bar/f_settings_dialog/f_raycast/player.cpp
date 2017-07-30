#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/player.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_components.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/animation_system.hpp"


//===========================================
// Player::Player
//===========================================
Player::Player(EntityManager& entityManager, double tallness, std::unique_ptr<Camera> camera)
  : m_entityManager(entityManager),
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
void Player::setPosition(const Point& pos) {
  m_camera->pos = pos;
}

//===========================================
// Player::move
//===========================================
void Player::move(const Vec2f& ds) {
  m_camera->pos = m_camera->pos + ds;
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
  AnimationSystem& animationSystem = m_entityManager
    .system<AnimationSystem>(ComponentKind::C_ANIMATION);

  animationSystem.playAnimation(sprite, "shoot", false);
}

//===========================================
// Player::camera
//===========================================
const Camera& Player::camera() const {
  return *m_camera;
}
