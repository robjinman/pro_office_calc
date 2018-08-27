#include "raycast/c_player_behaviour.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/time_service.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/animation_system.hpp"
#include "exception.hpp"
#include "utils.hpp"


//===========================================
// CPlayerBehaviour::CPlayerBehaviour
//===========================================
CPlayerBehaviour::CPlayerBehaviour(entityId_t entityId, EntityManager& entityManager,
  TimeService& timeService)
  : CBehaviour(entityId),
    SystemAccessor(entityManager),
    m_entityManager(entityManager),
    m_timeService(timeService) {}

//===========================================
// CPlayerBehaviour::update
//===========================================
void CPlayerBehaviour::update() {}

//===========================================
// CPlayerBehaviour::onEntityDamaged
//===========================================
void CPlayerBehaviour::onEntityDamaged(const GameEvent& e_) {
  auto& e = dynamic_cast<const EEntityDamaged&>(e_);
  Player& player = *spatialSys().sg.player;

#ifdef DEBUG
  CDamage& damage = m_entityManager.getComponent<CDamage>(player.body, ComponentKind::C_DAMAGE);
  DBG_PRINT("Player health: " << damage.health << "\n");
#endif

  if (e.health < e.prevHealth && player.red == -1) {
    player.red = Component::getNextId();

    double maxAlpha = 80;
    CColourOverlay* overlay = new CColourOverlay(player.red, QColor(200, 0, 0, maxAlpha),
      Point(0, 0), renderSys().viewport(), 10);

    renderSys().addComponent(pCRender_t(overlay));

    double duration = 0.33;
    int da = maxAlpha / (duration * m_timeService.frameRate);

    m_timeService.addTween(Tween{[=](long, double, double) -> bool {
      int alpha = overlay->colour.alpha() - da;
      overlay->colour.setAlpha(alpha);

      return alpha > 0;
    }, [&](long, double, double) {
      m_entityManager.deleteEntity(player.red);
      player.red = -1;
    }}, "redFade");
  }
}

//===========================================
// CPlayerBehaviour::onEntityDestroyed
//===========================================
void CPlayerBehaviour::onEntityDestroyed() {
  Player& player = *spatialSys().sg.player;

  if (!player.invincible) {
    player.alive = false;

    entityId_t entityId = Component::getNextId();

    CColourOverlay* overlay = new CColourOverlay(entityId, QColor(200, 0, 0, 80), Point(0, 0),
      renderSys().viewport());

    renderSys().addComponent(pCRender_t(overlay));

    double y = player.feetHeight();
    double h = player.getTallness();

    player.setFeetHeight(y + 0.6 * h);
    player.changeTallness(-0.6 * h);

    m_entityManager.deleteEntity(player.sprite);
    m_entityManager.deleteEntity(player.crosshair);

    m_entityManager.broadcastEvent(GameEvent{"player_death"});
  }
}

//===========================================
// CPlayerBehaviour::onPlayerMove
//===========================================
void CPlayerBehaviour::onPlayerMove() {
  Player& player = *spatialSys().sg.player;

  if (animationSys().animationState(player.body, "run") == AnimState::STOPPED) {
    animationSys().playAnimation(player.body, "run", false);
  }
}

//===========================================
// CPlayerBehaviour::onAnimationFinished
//===========================================
void CPlayerBehaviour::onAnimationFinished(const GameEvent& e_) {
  const EAnimationFinished& e = dynamic_cast<const EAnimationFinished&>(e_);
  Player& player = *spatialSys().sg.player;

  if (e.animName == "run") {
    animationSys().playAnimation(player.body, "idle", true);
  }
}

//===========================================
// CPlayerBehaviour::handleTargetedEvent
//===========================================
void CPlayerBehaviour::handleTargetedEvent(const GameEvent& e_) {
  if (e_.name == "entity_damaged") {
    onEntityDamaged(e_);
  }
  else if (e_.name == "entity_destroyed") {
    onEntityDestroyed();
  }
  else if (e_.name == "player_move") {
    onPlayerMove();
  }
  else if (e_.name == "animation_finished") {
    onAnimationFinished(e_);
  }
}
