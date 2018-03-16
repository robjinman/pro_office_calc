#include "fragments/relocatable/f_calculator/f_calculator.hpp"
#include "fragments/relocatable/f_calculator/f_calculator_spec.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"
#include "event_system.hpp"
#include "update_loop.hpp"
#include "utils.hpp"
#include "effects.hpp"


//===========================================
// FCalculator::FCalculator
//===========================================
FCalculator::FCalculator(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FCalculator", parent_, parentData_, m_data, commonData) {

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

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  QFont f = font();
  f.setPointSize(16);
  setFont(f);

  m_data.wgtDigitDisplay = makeQtObjPtr<QLineEdit>(this);
  m_data.wgtDigitDisplay->setMaximumHeight(40);
  m_data.wgtDigitDisplay->setAlignment(Qt::AlignRight);
  m_data.wgtDigitDisplay->setReadOnly(true);

  m_data.wgtButtonGrid = makeQtObjPtr<ButtonGrid>(this);

  delete m_data.vbox.get();

  m_data.vbox = makeQtObjPtr<QVBoxLayout>();
  m_data.vbox->addWidget(m_data.wgtDigitDisplay.get());
  m_data.vbox->addWidget(m_data.wgtButtonGrid.get());
  setLayout(m_data.vbox.get());

  connect(m_data.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this, SLOT(onButtonClick(int)));

  auto& spec = dynamic_cast<const FCalculatorSpec&>(spec_);
  setColour(*m_data.wgtDigitDisplay, spec.displayColour, QPalette::Base);
}

//===========================================
// FCalculator::cleanUp
//===========================================
void FCalculator::cleanUp() {
  DBG_PRINT("FCalculator::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);
}

//===========================================
// FCalculator::onButtonClick
//===========================================
void FCalculator::onButtonClick(int id) {
  if (id <= 9) {
    m_data.calculator.number(id);
  }
  else {
    switch (id) {
      case BTN_PLUS:
        m_data.calculator.plus();
        break;
      case BTN_MINUS:
        m_data.calculator.minus();
        break;
      case BTN_TIMES:
        m_data.calculator.times();
        break;
      case BTN_DIVIDE:
        m_data.calculator.divide();
        break;
      case BTN_POINT:
        m_data.calculator.point();
        break;
      case BTN_EQUALS: {
        m_data.calculator.equals();
        break;
      }
      case BTN_CLEAR:
        m_data.calculator.clear();
        break;
    }
  }

  m_data.wgtDigitDisplay->setText(m_data.calculator.display().c_str());
}

//===========================================
// FCalculator::~FCalculator
//===========================================
FCalculator::~FCalculator() {
  DBG_PRINT("FCalculator::~FCalculator\n");
}
