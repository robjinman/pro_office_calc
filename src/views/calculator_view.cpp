#include "calculator_view.hpp"
#include "event_system.hpp"
#include "update_loop.hpp"


//===========================================
// CalculatorView::CalculatorView
//===========================================
void CalculatorView::CalculatorView(QWidget& widget, EventSystem& eventSystem,
  UpdateLoop& updateLoop) {

  m_wgtDigitDisplay.reset(new QLineEdit(&widget));
  m_wgtDigitDisplay->setMaximumHeight(40);
  m_wgtDigitDisplay->setAlignment(Qt::AlignRight);
  m_wgtDigitDisplay->setReadOnly(true);

  m_wgtButtonGrid.reset(new ButtonGrid(m_eventSystem, m_updateLoop, &widget));

  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->addWidget(m_wgtDigitDisplay.get());
  vbox->addWidget(m_wgtButtonGrid.get());
  widget.setLayout(vbox);
}

//===========================================
// CalculatorView::setup
//===========================================
void CalculatorView::setup(int rootState) {
  if (ltelte<int>(ST_NORMAL_CALCULATOR_0, rootState, ST_NORMAL_CALCULATOR_10)) {
    if (dynamic_cast<NormalCalculatorView*>(m_subview.get()) == nullptr) {
      m_subview.reset(new NormalCalculatorView);
    }
  }
  else {
    switch (rootState) {
      case ST_DANGER_INFINITY:
        if (dynamic_cast<DangerInfinityView*>(m_subview.get()) == nullptr) {
          m_subview.reset(new DangerInfinityView);
        }
        break;
      case ST_SHUFFLED_KEYS:
        if (dynamic_cast<ShuffledKeysView*>(m_subview.get()) == nullptr) {
          m_subview.reset(new ShuffledKeysView);
        }
        break;
      default:
        EXCEPTION("CalculatorView cannot be setup for rootState " << rootState);
        break;
    }
  }

  subview->setup(rootState);
}
