#include <cassert>
#include <map>
#include <QMainWindow>
#include <QPushButton>
#include <QApplication>
#include <QButtonGroup>
#include "fragments/relocatable/f_calculator/f_partial_calc/f_partial_calc.hpp"
#include "fragments/relocatable/f_calculator/f_partial_calc/f_partial_calc_spec.hpp"
#include "fragments/relocatable/f_calculator/f_calculator.hpp"
#include "fragments/f_main/f_app_dialog/f_app_dialog.hpp"
#include "fragments/f_main/f_app_dialog/f_procalc_setup/events.hpp"
#include "state_ids.hpp"
#include "utils.hpp"
#include "evasive_button.hpp"
#include "exploding_button.hpp"
#include "state_ids.hpp"


using std::string;
using making_progress::SetupCompleteEvent;


struct BtnDesc {
  QString text;
  int idx;
  int row;
  int col;
};

static const std::map<buttonId_t, BtnDesc> EVASIVE_BTNS = {
  { BTN_FIVE, { "5", 5, 2, 1 } }
};

static const std::map<buttonId_t, BtnDesc> EXPLODING_BTNS = {
  { BTN_THREE, { "3", 3, 3, 2 } },
  { BTN_MINUS, { "-", 11, 3, 3 } },
  { BTN_POINT, { ".", 14, 4, 1 } },
};

//===========================================
// FPartialCalc::FPartialCalc
//===========================================
FPartialCalc::FPartialCalc(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FPartialCalc", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FPartialCalc::FPartialCalc\n");

  m_hSetupComplete = commonData.eventSystem.listen("makingProgress/setupComplete",
    [this](const Event& event) {

    const SetupCompleteEvent& e = dynamic_cast<const SetupCompleteEvent&>(event);
    toggleFeatures(e.features);
  });

  m_hDialogClosed = commonData.eventSystem.listen("dialogClosed",
    [&commonData](const Event& event) {

    const DialogClosedEvent& e = dynamic_cast<const DialogClosedEvent&>(event);

    if (e.name == "procalcSetup") {
      commonData.eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_MAKING_PROGRESS, true)));
    }
  });
}

//===========================================
// FPartialCalc::toggleFeatures
//===========================================
void FPartialCalc::toggleFeatures(const std::set<buttonId_t>& features) {
  auto& parentData = parentFragData<FCalculatorData>();
  auto& group = *parentData.wgtCalculator->wgtButtonGrid->buttonGroup;

  for (auto it = EVASIVE_BTNS.begin(); it != EVASIVE_BTNS.end(); ++it) {
    int idx = it->second.idx;

    EvasiveButton& btn =
      dynamic_cast<EvasiveButton&>(*parentData.wgtCalculator->wgtButtonGrid->buttons[idx]);

    btn.reset();
  }

  parentData.wgtCalculator->wgtButtonGrid->grid->update();

  for (auto& button : parentData.wgtCalculator->wgtButtonGrid->buttons) {
    buttonId_t id = static_cast<buttonId_t>(group.id(button.get()));

    if (features.count(id) > 0) {
      button->show();
    }
    else {
      button->hide();
    }
  }

  parentData.wgtCalculator->wgtDigitDisplay->clear();
}

//===========================================
// addButton
//===========================================
static void addButton(QPushButton* btn, ButtonGrid& btnGrid, buttonId_t id, const BtnDesc& desc) {
  QSizePolicy sp = btn->sizePolicy();
  sp.setRetainSizeWhenHidden(true);
  btn->setMaximumHeight(60);
  btn->setSizePolicy(sp);

  btnGrid.grid->addWidget(btn, desc.row, desc.col);
  btnGrid.buttonGroup->addButton(btn, id);
  btnGrid.buttons[desc.idx] = makeQtObjPtrFromRawPtr<QPushButton>(btn);
}

//===========================================
// FPartialCalc::reload
//===========================================
void FPartialCalc::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FPartialCalc::reload\n");

  auto& parentData = parentFragData<FCalculatorData>();

  auto& wgtButtonGrid = *parentData.wgtCalculator->wgtButtonGrid;

  for (auto it = EXPLODING_BTNS.begin(); it != EXPLODING_BTNS.end(); ++it) {
    addButton(new ExplodingButton(&wgtButtonGrid, it->second.text, commonData.appConfig,
      commonData.updateLoop), wgtButtonGrid, it->first, it->second);
  }

  for (auto it = EVASIVE_BTNS.begin(); it != EVASIVE_BTNS.end(); ++it) {
    addButton(new EvasiveButton(it->second.text), wgtButtonGrid, it->first, it->second);
  }

  disconnect(parentData.wgtCalculator->wgtButtonGrid.get(), SIGNAL(buttonClicked(int)),
    parentData.wgtCalculator.get(), SLOT(onButtonClick(int)));
  connect(parentData.wgtCalculator->wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtCalculator->wgtButtonGrid.get(), SIGNAL(buttonPressed(int)), this,
    SLOT(onButtonPress(int)));
}

//===========================================
// FPartialCalc::cleanUp
//===========================================
void FPartialCalc::cleanUp() {
  DBG_PRINT("FPartialCalc::cleanUp\n");

  auto& parentData = parentFragData<FCalculatorData>();

  for (auto& button : parentData.wgtCalculator->wgtButtonGrid->buttons) {
    button->show();
  }

  disconnect(parentData.wgtCalculator->wgtButtonGrid.get(), SIGNAL(buttonPressed(int)), this,
    SLOT(onButtonPress(int)));
  disconnect(parentData.wgtCalculator->wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtCalculator->wgtButtonGrid.get(), SIGNAL(buttonClicked(int)),
    parentData.wgtCalculator.get(), SLOT(onButtonClick(int)));
}

//===========================================
// FPartialCalc:onButtonClick
//===========================================
void FPartialCalc::onButtonClick(int id) {
  if (EVASIVE_BTNS.count(static_cast<buttonId_t>(id)) > 0) {
    return;
  }

  auto& parentData = parentFragData<FCalculatorData>();

  parentData.wgtCalculator->onButtonClick(id);

  string display = parentData.wgtCalculator->wgtDigitDisplay->text().toStdString();
  commonData.eventSystem.fire(pEvent_t(new making_progress::ButtonPressEvent(display)));
}

//===========================================
// FPartialCalc:onButtonPress
//===========================================
void FPartialCalc::onButtonPress(int id) {
  if (EVASIVE_BTNS.count(static_cast<buttonId_t>(id)) == 0) {
    return;
  }

  auto& parentData = parentFragData<FCalculatorData>();

  parentData.wgtCalculator->onButtonClick(id);

  string display = parentData.wgtCalculator->wgtDigitDisplay->text().toStdString();
  commonData.eventSystem.fire(pEvent_t(new making_progress::ButtonPressEvent(display)));
}

//===========================================
// FPartialCalc::~FPartialCalc
//===========================================
FPartialCalc::~FPartialCalc() {
  DBG_PRINT("FPartialCalc::~FPartialCalc\n");
}
