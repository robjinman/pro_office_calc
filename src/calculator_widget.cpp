#include <QButtonGroup>
#include <QPushButton>
#include <QKeyEvent>
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
  f.setPixelSize(18);
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
// CalculatorWidget::keyPressEvent
//===========================================
void CalculatorWidget::keyPressEvent(QKeyEvent* event) {
  if (!handleKeypresses) {
    return;
  }

  if (ltelte<int>(Qt::Key_0, event->key(), Qt::Key_9)) {
    onButtonClick(event->key() - Qt::Key_0);
    return;
  }

  switch (event->key()) {
	case Qt::Key_Escape:
    case Qt::Key_Clear:
      onButtonClick(BTN_CLEAR);
      break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      onButtonClick(BTN_EQUALS);
      break;
    case Qt::Key_Plus:
      onButtonClick(BTN_PLUS);
      break;
    case Qt::Key_Minus:
      onButtonClick(BTN_MINUS);
      break;
    case Qt::Key_Slash:
      onButtonClick(BTN_DIVIDE);
      break;
    case Qt::Key_Asterisk:
      onButtonClick(BTN_TIMES);
      break;
    case Qt::Key_Period:
      onButtonClick(BTN_POINT);
      break;
    default:
      break;
  }
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
      case BTN_PLUS: {
        calculator.plus();
        wgtOpDisplay->setText(symbol);
        break;
      }
      case BTN_MINUS: {
        calculator.minus();
        wgtOpDisplay->setText(symbol);
        break;
      }
      case BTN_TIMES: {
        calculator.times();
        wgtOpDisplay->setText(symbol);
        break;
      }
      case BTN_DIVIDE: {
        calculator.divide();
        wgtOpDisplay->setText(symbol);
        break;
      }
      case BTN_POINT: {
        calculator.point();
        break;
      }
      case BTN_EQUALS: {
        calculator.equals();
        break;
      }
      case BTN_CLEAR: {
        calculator.clear();
        break;
      }
    }
  }

  wgtDigitDisplay->setText(calculator.display().c_str());

  m_eventSystem.fire(pEvent_t(new CalculatorButtonPressEvent(id, calculator)));
}

//===========================================
// CalculatorWidget::~CalculatorWidget
//===========================================
CalculatorWidget::~CalculatorWidget() {}
