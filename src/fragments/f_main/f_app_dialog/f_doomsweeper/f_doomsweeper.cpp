#include <chrono>
#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragments/f_main/f_app_dialog/f_doomsweeper/f_doomsweeper.hpp"
#include "fragments/f_main/f_app_dialog/f_doomsweeper/f_doomsweeper_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_doomsweeper/object_factory.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/events.hpp"
#include "raycast/render_system.hpp"
#include "utils.hpp"
#include "update_loop.hpp"
#include "app_config.hpp"


//===========================================
// FDoomsweeper::FDoomsweeper
//===========================================
FDoomsweeper::FDoomsweeper(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FDoomsweeper", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FDoomsweeper::FDoomsweeper\n");
}

//===========================================
// FDoomsweeper::reload
//===========================================
void FDoomsweeper::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FDoomsweeper::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  if (m_data.vbox) {
    delete m_data.vbox.release();
  }

  m_data.vbox = makeQtObjPtr<QVBoxLayout>();
  m_data.vbox->setSpacing(0);
  m_data.vbox->setContentsMargins(0, 0, 0, 0);

  setLayout(m_data.vbox.get());

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  m_data.wgtRaycast = makeQtObjPtr<RaycastWidget>(commonData.eventSystem);

  auto& rootFactory = m_data.wgtRaycast->rootFactory();
  auto& timeService = m_data.wgtRaycast->timeService();
  auto& entityManager = m_data.wgtRaycast->entityManager();
  auto& audioService = m_data.wgtRaycast->audioService();

  auto factory = new doomsweeper::ObjectFactory(rootFactory, entityManager, timeService,
    audioService);

  m_data.wgtRaycast->rootFactory().addFactory(pGameObjectFactory_t(factory));
  m_data.wgtRaycast->initialise(config::dataPath("doomsweeper/map.svg"));

  m_data.gameLogic.reset(new doomsweeper::GameLogic(commonData.eventSystem, entityManager,
    m_data.wgtRaycast->rootFactory(), *factory, timeService));

  m_hSetup = commonData.eventSystem.listen("doomsweeper/minesweeperSetupComplete",
    [=](const Event& e_) {

    auto& e = dynamic_cast<const doomsweeper::MinesweeperSetupEvent&>(e_);
    m_initFuture = m_data.gameLogic->initialise(e.mineCoords);

    DBG_PRINT("Initialising game logic...\n");

    commonData.updateLoop.add([this]() -> bool {
      return waitForInit();
    }, []() {
      DBG_PRINT("DONE initialising game logic\n");
    });
  });

  m_data.vbox->addWidget(m_data.wgtRaycast.get());
}

//===========================================
// FDoomsweeper::waitForInit
//===========================================
bool FDoomsweeper::waitForInit() {
  auto status = m_initFuture.wait_for(std::chrono::milliseconds(0));

  if (status == std::future_status::ready) {
    m_initFuture.get();

    m_data.wgtRaycast->start();
    return false;
  }

  return true;
}

//===========================================
// FDoomsweeper::cleanUp
//===========================================
void FDoomsweeper::cleanUp() {
  DBG_PRINT("FDoomsweeper::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
}

//===========================================
// FDoomsweeper::~FDoomsweeper
//===========================================
FDoomsweeper::~FDoomsweeper() {
  DBG_PRINT("FDoomsweeper::~FDoomsweeper\n");
}
