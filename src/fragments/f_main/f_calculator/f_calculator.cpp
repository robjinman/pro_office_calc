#include <QVBoxLayout>
#include "event_system.hpp"
#include "update_loop.hpp"
#include "effects.hpp"
#include "fragments/f_main/f_calculator/f_calculator.hpp"
#include "fragments/f_main/f_calculator/f_calculator_spec.hpp"
#include "fragments/f_main/f_main.hpp"


//===========================================
// FCalculator::FCalculator
//===========================================
FCalculator::FCalculator(Fragment& parent_, FragmentData& parentData_)
  : QWidget(nullptr),
    Fragment("FCalculator", parent_, parentData_, m_data) {

  auto& parent = parentFrag<FMain>();
  auto& parentData = parentFragData<FMainData>();

  m_data.window = &parent;

  parent.setCentralWidget(this);

  QWidget::setParent(&parent);
  m_data.eventSystem = &parentData.eventSystem;
  m_data.updateLoop = &parentData.updateLoop;

  m_data.wgtDigitDisplay.reset(new QLineEdit(this));
  m_data.wgtDigitDisplay->setMaximumHeight(40);
  m_data.wgtDigitDisplay->setAlignment(Qt::AlignRight);
  m_data.wgtDigitDisplay->setReadOnly(true);

  m_data.wgtButtonGrid.reset(new ButtonGrid(this));

  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->addWidget(m_data.wgtDigitDisplay.get());
  vbox->addWidget(m_data.wgtButtonGrid.get());
  setLayout(vbox);

  connect(m_data.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this, SLOT(onButtonClick(int)));
}

//===========================================
// FCalculator::rebuild
//===========================================
void FCalculator::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FCalculatorSpec&>(spec_);
  setColour(*m_data.wgtDigitDisplay, spec.displayColour, QPalette::Base);

  Fragment::rebuild(spec_);
}

//===========================================
// FCalculator::cleanUp
//===========================================
void FCalculator::cleanUp() {

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
