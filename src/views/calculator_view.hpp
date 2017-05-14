#ifndef __PROCALC_CALCULATOR_VIEW_HPP__
#define __PROCALC_CALCULATOR_VIEW_HPP__


#include <memory>
#include <QLineEdit>
#include "view.hpp"
#include "button_grid.hpp"


class EventSystem;
class UpdateLoop;

class CalculatorView : public View {
  public:
    CalculatorView(QWidget& widget, EventSystem& eventSystem, UpdateLoop& updateLoop);

    virtual void setup(int rootState) override;

  private:
    std::unique_ptr<QLineEdit> m_wgtDigitDisplay;
    std::unique_ptr<ButtonGrid> m_wgtButtonGrid;

    std::unique_ptr<View> m_subview;
};


#endif
