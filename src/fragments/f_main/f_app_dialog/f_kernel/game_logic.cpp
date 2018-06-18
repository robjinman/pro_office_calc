#include <sstream>
#include <vector>
#include <random>
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


using std::vector;
using std::set;
using std::string;
using std::stringstream;


namespace millennium_bug {


static std::mt19937 randEngine(randomSeed());


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

  m_eventSystem.listen("millenniumBug/minesweeperSetupComplete", [=](const Event& e_) {
    auto& e = dynamic_cast<const MinesweeperSetupEvent&>(e_);

    initialise(e.mineCoords);
  }, m_setupEventId);
}

//===========================================
// GameLogic::coords
//===========================================
void GameLogic::initialise(const set<Coord>& mineCoords) {
  m_objectFactory.firstPassComplete = true;

  vector<entityId_t> safeCells = {
    Component::getIdFromString("safe_cell_0"),
    Component::getIdFromString("safe_cell_1"),
    Component::getIdFromString("safe_cell_2")
  };

  std::uniform_int_distribution<int> randomSafeCell(0, safeCells.size() - 1);

  vector<entityId_t> unsafeCells = {
    Component::getIdFromString("unsafe_cell_0"),
    Component::getIdFromString("unsafe_cell_1"),
    Component::getIdFromString("unsafe_cell_2")
  };

  std::uniform_int_distribution<int> randomUnsafeCell(0, unsafeCells.size() - 1);

  entityId_t startCellId = Component::getIdFromString("start_cell");
  parser::pObject_t startCellObj(m_objectFactory.objects.at(startCellId)->clone());

  Point startCellPos = m_objectFactory.objectPositions.at(startCellId);

  m_rootFactory.constructObject("cell", -1, *startCellObj, m_objectFactory.region,
    m_objectFactory.parentTransform);

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (i == 0 && j == 0) {
        continue;
      }

      entityId_t cellId = -1;

      if (mineCoords.count(Coord{i, j})) {
        cellId = unsafeCells[randomUnsafeCell(randEngine)];
      }
      else {
        cellId = safeCells[randomSafeCell(randEngine)];
      }

      assert(cellId != -1);

      parser::pObject_t cellObj(m_objectFactory.objects.at(cellId)->clone());
      const Point& cellPos = m_objectFactory.objectPositions.at(cellId);
      const Matrix& cellTransform = cellObj->groupTransform;

      Point targetPos = startCellPos + Vec2f(1600.0 * j, 1600.0 * i);
      Matrix m(0, targetPos - cellPos);

      stringstream ss;
      ss << "cell_" << i << "_" << j;

      cellObj->dict["name"] = ss.str();
      cellObj->groupTransform = cellTransform * m;

      m_rootFactory.constructObject("cell", -1, *cellObj, m_objectFactory.region,
        m_objectFactory.parentTransform);
    }
  }

  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  auto& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  DBG_PRINT("Connecting zones...\n");
  spatialSystem.connectZones();
  DBG_PRINT("Connecting regions...\n");
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
  m_eventSystem.forget(m_setupEventId);
}


}
