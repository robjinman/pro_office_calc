#ifndef __PROCALC_FRAGMENTS_CALCULATOR_WIDGET_HPP__
#define __PROCALC_FRAGMENTS_CALCULATOR_WIDGET_HPP__


#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include "button_grid.hpp"
#include "calculator.hpp"
#include "qt_obj_ptr.hpp"
#include "event.hpp"


struct CalculatorButtonPressEvent : public Event {
  CalculatorButtonPressEvent(int buttonId, Calculator& calculator)
    : Event("calculatorButtonPress"),
      buttonId(buttonId),
      calculator(calculator) {}

    int buttonId;
    Calculator& calculator;
};


class QMainWindow;
class EventSystem;

class CalculatorWidget : public QWidget {
  Q_OBJECT

  public:
    CalculatorWidget(EventSystem& eventSystem);

    Calculator calculator;
    QtObjPtr<QVBoxLayout> vbox;
    QtObjPtr<QLineEdit> wgtDigitDisplay;
    QtObjPtr<QLabel> wgtOpDisplay;
    QtObjPtr<ButtonGrid> wgtButtonGrid;

    virtual ~CalculatorWidget() override;

  public slots:
    void onButtonClick(int id);

  private:
    EventSystem& m_eventSystem;
};


#endif
