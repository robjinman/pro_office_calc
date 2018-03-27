#include "calculator_widget.hpp"
#include "utils.hpp"


//===========================================
// CalculatorWidget::CalculatorWidget
//===========================================
CalculatorWidget::CalculatorWidget()
  : QWidget(nullptr) {

  QFont f = font();
  f.setPointSize(16);
  setFont(f);

  wgtDigitDisplay = makeQtObjPtr<QLineEdit>(this);
  wgtDigitDisplay->setMaximumHeight(40);
  wgtDigitDisplay->setAlignment(Qt::AlignRight);
  wgtDigitDisplay->setReadOnly(true);

  wgtButtonGrid = makeQtObjPtr<ButtonGrid>(this);

  delete vbox.get();

  vbox = makeQtObjPtr<QVBoxLayout>();
  vbox->addWidget(wgtDigitDisplay.get());
  vbox->addWidget(wgtButtonGrid.get());
  setLayout(vbox.get());

  connect(wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this, SLOT(onButtonClick(int)));
}

//===========================================
// CalculatorWidget::onButtonClick
//===========================================
void CalculatorWidget::onButtonClick(int id) {
  if (id <= 9) {
    calculator.number(id);
  }
  else {
    switch (id) {
      case BTN_PLUS:
        calculator.plus();
        break;
      case BTN_MINUS:
        calculator.minus();
        break;
      case BTN_TIMES:
        calculator.times();
        break;
      case BTN_DIVIDE:
        calculator.divide();
        break;
      case BTN_POINT:
        calculator.point();
        break;
      case BTN_EQUALS: {
        calculator.equals();
        break;
      }
      case BTN_CLEAR:
        calculator.clear();
        break;
    }
  }

  wgtDigitDisplay->setText(calculator.display().c_str());
}

//===========================================
// CalculatorWidget::~CalculatorWidget
//===========================================
CalculatorWidget::~CalculatorWidget() {}
