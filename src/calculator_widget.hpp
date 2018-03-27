#ifndef __PROCALC_FRAGMENTS_CALCULATOR_WIDGET_HPP__
#define __PROCALC_FRAGMENTS_CALCULATOR_WIDGET_HPP__


#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include "button_grid.hpp"
#include "calculator.hpp"
#include "qt_obj_ptr.hpp"


class QMainWindow;

class CalculatorWidget : public QWidget {
  Q_OBJECT

  public:
    CalculatorWidget();

    Calculator calculator;
    QtObjPtr<QVBoxLayout> vbox;
    QtObjPtr<QLineEdit> wgtDigitDisplay;
    QtObjPtr<ButtonGrid> wgtButtonGrid;

    virtual ~CalculatorWidget() override;

  public slots:
    void onButtonClick(int id);
};


#endif
