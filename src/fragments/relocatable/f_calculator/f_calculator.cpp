#include "fragments/relocatable/f_calculator/f_calculator.hpp"
#include "fragments/relocatable/f_calculator/f_calculator_spec.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"
#include "utils.hpp"
#include "effects.hpp"


//===========================================
// FCalculator::FCalculator
//===========================================
FCalculator::FCalculator(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FCalculator", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FCalculator::FCalculator\n");
}

//===========================================
// FCalculator::reload
//===========================================
void FCalculator::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FCalculator::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  m_data.wgtCalculator = makeQtObjPtr<CalculatorWidget>(commonData.eventSystem);

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(m_data.wgtCalculator.get());

  QFont f = m_data.wgtCalculator->font();
  f.setPixelSize(18);
  m_data.wgtCalculator->setFont(f);

  auto& spec = dynamic_cast<const FCalculatorSpec&>(spec_);
  setColour(*m_data.wgtCalculator->wgtDigitDisplay, spec.displayColour, QPalette::Base);
}

//===========================================
// FCalculator::cleanUp
//===========================================
void FCalculator::cleanUp() {
  DBG_PRINT("FCalculator::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(m_data.wgtCalculator.get());
}

//===========================================
// FCalculator::~FCalculator
//===========================================
FCalculator::~FCalculator() {
  DBG_PRINT("FCalculator::~FCalculator\n");
}
