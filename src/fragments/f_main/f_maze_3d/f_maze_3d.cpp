#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_maze_3d/f_maze_3d.hpp"
#include "fragments/f_main/f_maze_3d/f_maze_3d_spec.hpp"
#include "utils.hpp"
#include "app_config.hpp"
#include "event_system.hpp"


//===========================================
// FMaze3d::FMaze3d
//===========================================
FMaze3d::FMaze3d(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FMaze3d", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FMaze3d::FMaze3d\n");
}

//===========================================
// FMaze3d::reload
//===========================================
void FMaze3d::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FMaze3d::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  m_data.vbox = makeQtObjPtr<QVBoxLayout>();
  m_data.vbox->setSpacing(0);
  m_data.vbox->setContentsMargins(0, 0, 0, 0);

  setLayout(m_data.vbox.get());

  m_data.wgtRaycast = makeQtObjPtr<RaycastWidget>(commonData.eventSystem);
  m_data.vbox->addWidget(m_data.wgtRaycast.get());

#if PROFILING_ON
  double profileDuration = config::getDoubleArg(commonData.commandLineArgs, 1, -1);

  DBG_PRINT("Profile duration = " << profileDuration << " seconds...\n");

  if (profileDuration > 0) {
    const TimeService& timeService = m_data.wgtRaycast->timeService();
    double start = timeService.now();

    commonData.updateLoop.add([=, &timeService]() {
      if (timeService.now() - start > profileDuration) {
        commonData.eventSystem.fire(pEvent_t(new Event("quit")));
        return false;
      }
      return true;
    }, []() {});
  }
#endif

  m_data.wgtRaycast->initialise(config::dataPath("common/maps/house.svg"));
}

//===========================================
// FMaze3d::cleanUp
//===========================================
void FMaze3d::cleanUp() {
  DBG_PRINT("FMaze3d::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);
}

//===========================================
// FMaze3d::~FMaze3d
//===========================================
FMaze3d::~FMaze3d() {
  DBG_PRINT("FMaze3d::~FMaze3d\n");
}
