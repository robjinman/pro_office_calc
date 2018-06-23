#include <sstream>
#include <vector>
#include <random>
#include "fragments/f_main/f_app_dialog/f_kernel/game_logic.hpp"
#include "fragments/f_main/f_app_dialog/f_kernel/game_events.hpp"
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
#include "raycast/c_door_behaviour.hpp"
#include "event_system.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "utils.hpp"


using std::vector;
using std::set;
using std::string;
using std::stringstream;


namespace millennium_bug {


static const int ROWS = 8;
static const int COLS = 8;
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

  m_entityId = Component::getNextId();

  auto& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  CEventHandler* events = new CEventHandler(m_entityId);
  events->broadcastedEventHandlers.push_back(EventHandler{"player_enter_cell_inner",
    [this](const GameEvent& e_) {

    auto& e = dynamic_cast<const EPlayerEnterCellInner&>(e_);
    onPlayerEnterCellInner(e.cellId);
  }});
  events->broadcastedEventHandlers.push_back(EventHandler{"cell_door_opened",
    [this](const GameEvent& e_) {

    auto& e = dynamic_cast<const ECellDoorOpened&>(e_);
    onCellDoorOpened(e.cellId);
  }});

  eventHandlerSystem.addComponent(pComponent_t(events));
}

//===========================================
// GameLogic::initialise
//===========================================
void GameLogic::initialise(const set<Coord>& mineCoords) {
  m_objectFactory.firstPassComplete = true;

  vector<entityId_t> safeCells = {
    Component::getIdFromString("safe_cell_0"),
    //Component::getIdFromString("safe_cell_1"),
    //Component::getIdFromString("safe_cell_2")
  };

  std::uniform_int_distribution<int> randomSafeCell(0, safeCells.size() - 1);

  vector<entityId_t> unsafeCells = {
    Component::getIdFromString("unsafe_cell_0"),
    //Component::getIdFromString("unsafe_cell_1"),
    //Component::getIdFromString("unsafe_cell_2")
  };

  std::uniform_int_distribution<int> randomUnsafeCell(0, unsafeCells.size() - 1);

  entityId_t startCellId = Component::getIdFromString("start_cell");
  parser::pObject_t startCellObj(m_objectFactory.objects.at(startCellId)->clone());

  Point startCellPos = m_objectFactory.objectPositions.at(startCellId);

  m_rootFactory.constructObject("cell", -1, *startCellObj, m_objectFactory.region,
    m_objectFactory.parentTransform);

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      if (i == 0 && j == 0) {
        sealDoor(m_objectFactory.cellDoors[startCellId].north);
        sealDoor(m_objectFactory.cellDoors[startCellId].west);
        continue;
      }

      entityId_t protoCellId = -1;

      if (mineCoords.count(Coord{i, j})) {
        protoCellId = unsafeCells[randomUnsafeCell(randEngine)];
      }
      else {
        protoCellId = safeCells[randomSafeCell(randEngine)];
      }

      assert(protoCellId != -1);

      parser::pObject_t cellObj(m_objectFactory.objects.at(protoCellId)->clone());
      const Point& cellPos = m_objectFactory.objectPositions.at(protoCellId);
      const Matrix& cellTransform = cellObj->groupTransform;

      Point targetPos = startCellPos + Vec2f(1600.0 * j, 1600.0 * i);
      Matrix m(0, targetPos - cellPos);

      stringstream ss;
      ss << "cell_" << i << "_" << j;

      entityId_t cellId = Component::getIdFromString(ss.str());
      m_cellIds[cellId] = Coord{i, j};

      cellObj->dict["name"] = ss.str();
      cellObj->groupTransform = cellTransform * m;

      m_rootFactory.constructObject("cell", -1, *cellObj, m_objectFactory.region,
        m_objectFactory.parentTransform);

      if (i == 0) {
        sealDoor(m_objectFactory.cellDoors[cellId].north);
      }

      if (i + 1 == ROWS) {
        sealDoor(m_objectFactory.cellDoors[cellId].south);
      }

      if (j == 0) {
        sealDoor(m_objectFactory.cellDoors[cellId].west);
      }

      if (j + 1 == COLS) {
        sealDoor(m_objectFactory.cellDoors[cellId].east);
      }
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
// GameLogic::sealDoor
//===========================================
void GameLogic::sealDoor(entityId_t doorId) {
  auto& region = m_entityManager.getComponent<CRegion>(doorId, ComponentKind::C_RENDER);
  auto& behaviour =
    m_entityManager.getComponent<CDoorBehaviour>(doorId, ComponentKind::C_BEHAVIOUR);

  DBG_PRINT("Sealing door with id " << doorId << "\n");

  for (auto& pBoundary : region.boundaries) {
    if (pBoundary->kind == CRenderKind::JOIN) {
      CJoin& join = dynamic_cast<CJoin&>(*pBoundary);

      join.topTexture = "slimy_bricks";
      join.bottomTexture = "slimy_bricks";
    }
  }

  behaviour.isPlayerActivated = false;

  auto& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);
  focusSystem.removeEntity(doorId);
}

//===========================================
// GameLogic::onPlayerEnterCellInner
//===========================================
void GameLogic::onPlayerEnterCellInner(entityId_t cellId) {
  Coord coord = m_cellIds[cellId];
  m_eventSystem.fire(pEvent_t(new InnerCellEnteredEvent{coord}));
}

//===========================================
// GameLogic::onCellDoorOpened
//===========================================
void GameLogic::onCellDoorOpened(entityId_t cellId) {
  if (m_history.newest() != cellId) {
    m_history.push(cellId);
  }

  lockDoors();
}

//===========================================
// GameLogic::lockDoors
//===========================================
void GameLogic::lockDoors() {
#ifdef DEBUG
  std::cout << "History: ";
  for (int i = 0; i < m_history.size(); ++i) {
    std::cout << "(" << m_cellIds[m_history.nthNewest(i)] << ") ";
  }
  std::cout << "\n";
#endif

  if (m_history.size() >= 2) {
    entityId_t prevCellId = m_history.nthNewest(1);
    auto& doors = m_objectFactory.cellDoors[prevCellId];

    DBG_PRINT("Locking doors of cell " << m_cellIds[prevCellId] << "\n");

    for (entityId_t door : doors) {
      DBG_PRINT("Locking door with id " << door << "\n");

      auto& behaviour =
        m_entityManager.getComponent<CDoorBehaviour>(door, ComponentKind::C_BEHAVIOUR);

      behaviour.isPlayerActivated = false;
    }
  }

  if (m_history.size() >= 3) {
    entityId_t prevPrevCellId = m_history.nthNewest(2);
    auto& doors = m_objectFactory.cellDoors[prevPrevCellId];

    DBG_PRINT("Unlocking doors of cell " << m_cellIds[prevPrevCellId] << "\n");

    for (entityId_t door : doors) {
      DBG_PRINT("Unlocking door with id " << door << "\n");

      auto& behaviour =
        m_entityManager.getComponent<CDoorBehaviour>(door, ComponentKind::C_BEHAVIOUR);

      behaviour.isPlayerActivated = true;
    }
  }
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {
  m_eventSystem.forget(m_setupEventId);
}


}
