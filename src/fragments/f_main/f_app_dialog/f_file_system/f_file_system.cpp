#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragments/f_main/f_app_dialog/f_file_system/f_file_system.hpp"
#include "fragments/f_main/f_app_dialog/f_file_system/f_file_system_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_file_system/object_factory.hpp"
#include "raycast/render_system.hpp"
#include "utils.hpp"
#include "event_system.hpp"
#include "app_config.hpp"


//===========================================
// FFileSystem::FFileSystem
//===========================================
FFileSystem::FFileSystem(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FFileSystem", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FFileSystem::FFileSystem\n");
}

//===========================================
// FFileSystem::reload
//===========================================
void FFileSystem::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FFileSystem::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_data.vbox = makeQtObjPtr<QVBoxLayout>();
  m_data.vbox->setSpacing(0);
  m_data.vbox->setContentsMargins(0, 0, 0, 0);

  setLayout(m_data.vbox.get());

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  m_data.wgtRaycast = makeQtObjPtr<RaycastWidget>(commonData.appConfig, commonData.eventSystem);

  auto& rootFactory = m_data.wgtRaycast->rootFactory();
  auto& timeService = m_data.wgtRaycast->timeService();
  auto& entityManager = m_data.wgtRaycast->entityManager();
  auto& audioService = m_data.wgtRaycast->audioService();

  GameObjectFactory* factory = new going_in_circles::ObjectFactory(rootFactory, entityManager,
    timeService, audioService);

  m_data.wgtRaycast->rootFactory().addFactory(pGameObjectFactory_t(factory));
  m_data.wgtRaycast->initialise(commonData.appConfig.dataPath("going_in_circles/map.svg"));

  m_data.gameLogic.reset(new going_in_circles::GameLogic(commonData.eventSystem, entityManager));

  m_data.wgtRaycast->start();

  m_data.vbox->addWidget(m_data.wgtRaycast.get());
}

//===========================================
// FFileSystem::cleanUp
//===========================================
void FFileSystem::cleanUp() {
  DBG_PRINT("FFileSystem::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
}

//===========================================
// FFileSystem::~FFileSystem
//===========================================
FFileSystem::~FFileSystem() {
  DBG_PRINT("FFileSystem::~FFileSystem\n");
}
