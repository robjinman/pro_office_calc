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
// CPlayerBehaviour::handleEvent
//===========================================
void CPlayerBehaviour::handleEvent(const GameEvent& e) {
  if (e.name == "entity_damaged") {
    auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
    auto& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

    Player& player = *spatialSystem.sg.player;
    CDamage& damage = m_entityManager.getComponent<CDamage>(player.body, ComponentKind::C_DAMAGE);

    const EEntityDamaged& event = dynamic_cast<const EEntityDamaged&>(e);

    if (event.entityId == player.body) {
      DBG_PRINT("Player health: " << damage.health << "\n");

      if (player.red == -1) {
        player.red = Component::getNextId();

        double maxAlpha = 80;
        CColourOverlay* overlay = new CColourOverlay(player.red, QColor(200, 0, 0, maxAlpha),
          Point(0, 0), renderSystem.rg.viewport);

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
  }
}
