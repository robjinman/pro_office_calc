#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragments/f_main/f_app_dialog/f_app_dialog.hpp"
#include "fragments/f_main/f_app_dialog/f_file_system_2/f_file_system_2.hpp"
#include "fragments/f_main/f_app_dialog/f_file_system_2/f_file_system_2_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_file_system_2/object_factory.hpp"
#include "raycast/render_system.hpp"
#include "utils.hpp"
#include "app_config.hpp"
#include "state_ids.hpp"


//===========================================
// FFileSystem2::FFileSystem2
//===========================================
FFileSystem2::FFileSystem2(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FFileSystem2", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FFileSystem2::FFileSystem2\n");

  m_hDialogClosed = commonData.eventSystem.listen("dialogClosed",
    [&commonData](const Event& event) {

    const DialogClosedEvent& e = dynamic_cast<const DialogClosedEvent&>(event);

    if (e.name == "fileBrowser") {
      commonData.eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_T_MINUS_TWO_MINUTES,
        true)));
    }
  });
}

//===========================================
// FFileSystem2::reload
//===========================================
void FFileSystem2::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FFileSystem2::reload\n");

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

  m_data.wgtRaycast = makeQtObjPtr<RaycastWidget>(commonData.appConfig, commonData.eventSystem);

  auto& rootFactory = m_data.wgtRaycast->rootFactory();
  auto& timeService = m_data.wgtRaycast->timeService();
  auto& entityManager = m_data.wgtRaycast->entityManager();
  auto& audioService = m_data.wgtRaycast->audioService();

  GameObjectFactory* factory = new t_minus_two_minutes::ObjectFactory(rootFactory, entityManager,
    timeService, audioService);

  m_data.wgtRaycast->rootFactory().addFactory(pGameObjectFactory_t(factory));
  m_data.wgtRaycast->initialise(commonData.appConfig.dataPath("t_minus_two_minutes/map.svg"));

  m_data.gameLogic.reset(new t_minus_two_minutes::GameLogic(commonData.eventSystem, audioService,
    timeService, entityManager));

  m_data.wgtRaycast->start();

  m_data.vbox->addWidget(m_data.wgtRaycast.get());
}

//===========================================
// FFileSystem2::cleanUp
//===========================================
void FFileSystem2::cleanUp() {
  DBG_PRINT("FFileSystem2::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
}

//===========================================
// FFileSystem2::~FFileSystem2
//===========================================
FFileSystem2::~FFileSystem2() {
  DBG_PRINT("FFileSystem2::~FFileSystem2\n");
}
