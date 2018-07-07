#include "raycast/c_player_behaviour.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/time_service.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/damage_system.hpp"
#include "exception.hpp"
#include "utils.hpp"


//===========================================
// CPlayerBehaviour::CPlayerBehaviour
//===========================================
CPlayerBehaviour::CPlayerBehaviour(entityId_t entityId, EntityManager& entityManager,
  TimeService& timeService)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_timeService(timeService) {}

//===========================================
// CPlayerBehaviour::update
//===========================================
void CPlayerBehaviour::update() {

}

//===========================================
// CPlayerBehaviour::handleTargetedEvent
//===========================================
void CPlayerBehaviour::handleTargetedEvent(const GameEvent& e_) {
  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  auto& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  Player& player = *spatialSystem.sg.player;

  if (e_.name == "entity_damaged") {
    auto& e = dynamic_cast<const EEntityDamaged&>(e_);

    CDamage& damage = m_entityManager.getComponent<CDamage>(player.body, ComponentKind::C_DAMAGE);

    DBG_PRINT("Player health: " << damage.health << "\n");

    if (e.health < e.prevHealth && player.red == -1) {
      player.red = Component::getNextId();

      double maxAlpha = 80;
      CColourOverlay* overlay = new CColourOverlay(player.red, QColor(200, 0, 0, maxAlpha),
        Point(0, 0), renderSystem.rg.viewport, 10);

      renderSystem.addComponent(pCRender_t(overlay));

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
  else if (e_.name == "entity_destroyed") {
    if (!player.invincible) {
      player.alive = false;

      entityId_t entityId = Component::getNextId();

      CColourOverlay* overlay = new CColourOverlay(entityId, QColor(200, 0, 0, 80), Point(0, 0),
        renderSystem.rg.viewport);

      renderSystem.addComponent(pCRender_t(overlay));

      double y = player.feetHeight();
      double h = player.getTallness();

      player.setFeetHeight(y + 0.6 * h);
      player.changeTallness(-0.6 * h);

      m_entityManager.deleteEntity(player.sprite);
      m_entityManager.deleteEntity(player.crosshair);

      m_entityManager.broadcastEvent(GameEvent{"player_death"});
    }
  }
}
