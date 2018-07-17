#include <QButtonGroup>
#include <QPushButton>
#include "calculator_widget.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// CalculatorWidget::CalculatorWidget
//===========================================
CalculatorWidget::CalculatorWidget(EventSystem& eventSystem)
  : QWidget(nullptr),
    m_eventSystem(eventSystem) {

  QFont f = font();
  f.setPointSize(16);
  setFont(f);

  wgtDigitDisplay = makeQtObjPtr<QLineEdit>(this);
  wgtDigitDisplay->setMaximumHeight(40);
  wgtDigitDisplay->setAlignment(Qt::AlignRight);
  wgtDigitDisplay->setReadOnly(true);

  wgtButtonGrid = makeQtObjPtr<ButtonGrid>(this);

  wgtOpDisplay = makeQtObjPtr<QLabel>(this);
  wgtOpDisplay->setFixedHeight(18);
  wgtOpDisplay->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

  if (vbox) {
    delete vbox.release();
  }

  vbox = makeQtObjPtr<QVBoxLayout>();
  vbox->setSpacing(0);
  vbox->addWidget(wgtDigitDisplay.get());
  vbox->addWidget(wgtOpDisplay.get());
  vbox->addWidget(wgtButtonGrid.get());
  setLayout(vbox.get());

  connect(wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this, SLOT(onButtonClick(int)));
}

//===========================================
// CalculatorWidget::onButtonClick
//===========================================
void CalculatorWidget::onButtonClick(int id) {
  wgtOpDisplay->setText("");

  if (id <= 9) {
    calculator.number(id);
  }
  else {
    auto btn = dynamic_cast<const QPushButton*>(wgtButtonGrid->buttonGroup->button(id));
    QString symbol = btn->text();

    switch (id) {
      case BTN_PLUS:
        calculator.plus();
        wgtOpDisplay->setText(symbol);
        break;
      case BTN_MINUS:
        calculator.minus();
        wgtOpDisplay->setText(symbol);
        break;
      case BTN_TIMES:
        calculator.times();
        wgtOpDisplay->setText(symbol);
        break;
      case BTN_DIVIDE:
        calculator.divide();
        wgtOpDisplay->setText(symbol);
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

  m_eventSystem.fire(pEvent_t(new CalculatorButtonPressEvent(id, calculator)));
}

//===========================================
// CalculatorWidget::~CalculatorWidget
//===========================================
CalculatorWidget::~CalculatorWidget() {}
