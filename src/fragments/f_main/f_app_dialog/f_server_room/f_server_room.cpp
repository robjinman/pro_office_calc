#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragments/f_main/f_app_dialog/f_server_room/f_server_room.hpp"
#include "fragments/f_main/f_app_dialog/f_server_room/f_server_room_spec.hpp"
#include "utils.hpp"
#include "event_system.hpp"


//===========================================
// FServerRoom::FServerRoom
//===========================================
FServerRoom::FServerRoom(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FServerRoom", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FServerRoom::FServerRoom\n");
}

//===========================================
// FServerRoom::reload
//===========================================
void FServerRoom::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FServerRoom::reload\n");

  commonData.eventSystem.listen("launchServerRoom", [this, &spec_](const Event&) {
    auto& parentData = parentFragData<WidgetFragData>();

    m_data.vbox = makeQtObjPtr<QVBoxLayout>();
    m_data.vbox->setSpacing(0);
    m_data.vbox->setContentsMargins(0, 0, 0, 0);

    setLayout(m_data.vbox.get());

    m_origParentState.spacing = parentData.box->spacing();
    m_origParentState.margins = parentData.box->contentsMargins();

    for (int i = 0; i < parentData.box->count(); ++i) {
      QWidget* wgt = parentData.box->itemAt(i)->widget();
      if (wgt) {
        wgt->hide();
      }
    }

    parentData.box->setSpacing(0);
    parentData.box->setContentsMargins(0, 0, 0, 0);
    parentData.box->addWidget(this);

    m_data.wgtRaycast = makeQtObjPtr<RaycastWidget>(commonData.eventSystem);
    m_data.vbox->addWidget(m_data.wgtRaycast.get());

    m_data.wgtRaycast->initialise("data/maps/house.svg");
  }, m_launchEventId);
}

//===========================================
// FServerRoom::cleanUp
//===========================================
void FServerRoom::cleanUp() {
  DBG_PRINT("FServerRoom::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);

  for (int i = 0; i < parentData.box->count(); ++i) {
    QWidget* wgt = parentData.box->itemAt(i)->widget();
    if (wgt) {
      wgt->show();
    }
  }

  commonData.eventSystem.forget(m_launchEventId);
}

//===========================================
// FServerRoom::~FServerRoom
//===========================================
FServerRoom::~FServerRoom() {
  DBG_PRINT("FServerRoom::~FServerRoom\n");
}
