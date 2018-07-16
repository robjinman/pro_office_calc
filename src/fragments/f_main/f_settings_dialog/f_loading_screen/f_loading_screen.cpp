#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_settings_dialog/f_loading_screen/f_loading_screen.hpp"
#include "fragments/f_main/f_settings_dialog/f_loading_screen/f_loading_screen_spec.hpp"
#include "utils.hpp"


//===========================================
// FLoadingScreen::FLoadingScreen
//===========================================
FLoadingScreen::FLoadingScreen(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QLabel(nullptr),
    Fragment("FLoadingScreen", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FLoadingScreen::FLoadingScreen\n");
}

//===========================================
// FLoadingScreen::reload
//===========================================
void FLoadingScreen::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FLoadingScreen::reload\n");

  auto& spec = dynamic_cast<const FLoadingScreenSpec&>(spec_);

  auto& parentData = parentFragData<FSettingsDialogData>();

  m_origParentState.spacing = parentData.vbox->spacing();
  m_origParentState.margins = parentData.vbox->contentsMargins();

  parentData.vbox->setSpacing(0);
  parentData.vbox->setContentsMargins(0, 0, 0, 0);
  parentData.vbox->addWidget(this);

  updateGeometry();

  m_data.label = makeQtObjPtr<QLabel>(this);
  m_data.label->setText(spec.targetValue.c_str());

  QPalette p = m_data.label->palette();
  p.setColor(QPalette::WindowText, Qt::white);
  m_data.label->setPalette(p);

  QFont font = m_data.label->font();
  font.setPixelSize(20);
  m_data.label->setFont(font);

  m_data.label->move(168, 59);
}

//===========================================
// FLoadingScreen::cleanUp
//===========================================
void FLoadingScreen::cleanUp() {
  DBG_PRINT("FLoadingScreen::cleanUp\n");

  auto& parentData = parentFragData<FSettingsDialogData>();

  parentData.vbox->setSpacing(m_origParentState.spacing);
  parentData.vbox->setContentsMargins(m_origParentState.margins);
  parentData.vbox->removeWidget(this);
}

//===========================================
// FLoadingScreen::~FLoadingScreen
//===========================================
FLoadingScreen::~FLoadingScreen() {
  DBG_PRINT("FLoadingScreen::~FLoadingScreen\n");
}
