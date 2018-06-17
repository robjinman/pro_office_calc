#include <sstream>
#include "fragments/f_main/f_app_dialog/f_kernel/game_logic.hpp"
#include "fragments/f_main/f_app_dialog/f_kernel/object_factory.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/root_factory.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/agent_system.hpp"
#include "event_system.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "utils.hpp"


using std::string;
using std::stringstream;


namespace millennium_bug {


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(EventSystem& eventSystem, EntityManager& entityManager,
  RootFactory& rootFactory, ObjectFactory& objectFactory)
  : m_eventSystem(eventSystem),
    m_entityManager(entityManager),
    m_rootFactory(rootFactory),
    m_objectFactory(objectFactory) {

  DBG_PRINT("GameLogic::GameLogic\n");

  m_objectFactory.firstPassComplete = true;

  entityId_t safeCell0Id = Component::getIdFromString("safe_cell_0");
  parser::pObject_t safeCell0Obj(m_objectFactory.objects.at(safeCell0Id)->clone());

  entityId_t startCellId = Component::getIdFromString("start_cell");
  parser::pObject_t startCellObj(m_objectFactory.objects.at(startCellId)->clone());

  m_rootFactory.constructObject("cell", -1, *startCellObj, m_objectFactory.region,
    m_objectFactory.parentTransform);

  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      if (i == 0 && j == 0) {
        continue;
      }

      Matrix m(0, Vec2f(800.0 * j, 800.0 * i));

      stringstream ss;
      ss << "cell_" << i << "_" << j;

      safeCell0Obj->dict["name"] = ss.str();
      safeCell0Obj->groupTransform = startCellObj->groupTransform * m;

      m_rootFactory.constructObject("cell", -1, *safeCell0Obj, m_objectFactory.region,
        m_objectFactory.parentTransform);
    }
  }

  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  auto& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  spatialSystem.connectZones();
  renderSystem.connectRegions();

  entityId_t playerId = Component::getIdFromString("player");
  entityId_t startPointId = Component::getIdFromString("minesweeper_start_point");

  auto& startPoint = dynamic_cast<const CVRect&>(spatialSystem.getComponent(startPointId));

  spatialSystem.relocateEntity(playerId, *startPoint.zone, startPoint.pos);
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {

}


}
