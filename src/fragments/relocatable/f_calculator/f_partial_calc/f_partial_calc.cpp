#include <cassert>
#include <QMainWindow>
#include <QPushButton>
#include <QApplication>
#include <QButtonGroup>
#include "fragments/relocatable/f_calculator/f_partial_calc/f_partial_calc.hpp"
#include "fragments/relocatable/f_calculator/f_partial_calc/f_partial_calc_spec.hpp"
#include "fragments/relocatable/f_calculator/f_calculator.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FPartialCalc::FPartialCalc
//===========================================
FPartialCalc::FPartialCalc(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FPartialCalc", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FPartialCalc::FPartialCalc\n");

  commonData.eventSystem.listen("makingProgress/setupComplete", [this](const Event& event) {
    const SetupCompleteEvent& e = dynamic_cast<const SetupCompleteEvent&>(event);
    toggleFeatures(e.features);
  }, m_eventIdx);
}

//===========================================
// FPartialCalc::toggleFeatures
//===========================================
void FPartialCalc::toggleFeatures(const std::set<buttonId_t>& features) {
  auto& parentData = parentFragData<FCalculatorData>();
  auto& group = *parentData.wgtButtonGrid->buttonGroup;

  for (auto& button : parentData.wgtButtonGrid->buttons) {
    buttonId_t id = static_cast<buttonId_t>(group.id(button.get()));

    if (features.count(id) > 0) {
      button->show();
    }
    else {
      button->hide();
    }
  }
}

//===========================================
// FPartialCalc::reload
//===========================================
void FPartialCalc::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FPartialCalc::reload\n");

  auto& parentData = parentFragData<FCalculatorData>();
  auto& parent = parentFrag<FCalculator>();

  disconnect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), &parent,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));
}

//===========================================
// FPartialCalc::cleanUp
//===========================================
void FPartialCalc::cleanUp() {
  DBG_PRINT("FPartialCalc::cleanUp\n");

  auto& parentData = parentFragData<FCalculatorData>();
  auto& parent = parentFrag<FCalculator>();

  for (auto& button : parentData.wgtButtonGrid->buttons) {
    button->show();
  }

  disconnect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), &parent,
    SLOT(onButtonClick(int)));

  commonData.eventSystem.forget(m_eventIdx);
}

//===========================================
// FPartialCalc:onButtonClick
//===========================================
void FPartialCalc::onButtonClick(int id) {
  parentFrag<FCalculator>().onButtonClick(id);

}

//===========================================
// FPartialCalc::~FPartialCalc
//===========================================
FPartialCalc::~FPartialCalc() {
  DBG_PRINT("FPartialCalc::~FPartialCalc\n");
}
