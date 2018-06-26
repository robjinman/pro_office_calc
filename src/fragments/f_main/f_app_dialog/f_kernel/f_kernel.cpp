#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragments/f_main/f_app_dialog/f_kernel/f_kernel.hpp"
#include "fragments/f_main/f_app_dialog/f_kernel/f_kernel_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_kernel/object_factory.hpp"
#include "raycast/render_system.hpp"
#include "utils.hpp"
#include "event_system.hpp"
#include "app_config.hpp"


//===========================================
// FKernel::FKernel
//===========================================
FKernel::FKernel(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FKernel", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FKernel::FKernel\n");
}

//===========================================
// FKernel::reload
//===========================================
void FKernel::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FKernel::reload\n");

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

  m_data.vbox->addWidget(m_data.wgtRaycast.get());
}

//===========================================
// FKernel::cleanUp
//===========================================
void FKernel::cleanUp() {
  DBG_PRINT("FKernel::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
}

//===========================================
// FKernel::~FKernel
//===========================================
FKernel::~FKernel() {
  DBG_PRINT("FKernel::~FKernel\n");
}
