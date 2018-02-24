#include <cassert>
#include <QMainWindow>
#include <QPushButton>
#include <QApplication>
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

  disconnect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), &parent,
    SLOT(onButtonClick(int)));
}

//===========================================
// FPartialCalc:onButtonClick
//===========================================
void FPartialCalc::onButtonClick(int id) {
  parentFrag<FCalculator>().onButtonClick(id);

  DBG_PRINT("Hello from FPartialCalc\n");
}

//===========================================
// FPartialCalc::~FPartialCalc
//===========================================
FPartialCalc::~FPartialCalc() {
  DBG_PRINT("FPartialCalc::~FPartialCalc\n");
}
