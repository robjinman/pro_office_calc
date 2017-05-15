#include <QLineEdit>
#include <QVBoxLayout>
#include "event_system.hpp"
#include "update_loop.hpp"
#include "calculator.hpp"
#include "exception.hpp"
#include "fragments/normal_calculator_fragment.hpp"
#include "fragments/shuffled_keys_fragment.hpp"
#include "fragments/calculator_fragment.hpp"
#include "fragments/specs/calculator_fragment_spec.hpp"


//===========================================
// CalculatorFragment::CalculatorFragment
//===========================================
CalculatorFragment::CalculatorFragment(QWidget* parent, QMenuBar& menuBar, QAction& actAbout,
  EventSystem& eventSystem, UpdateLoop& updateLoop)
  : m_menuBar(menuBar),
    m_actAbout(actAbout),
    m_eventSystem(eventSystem),
    m_updateLoop(updateLoop) {

  m_wgtDigitDisplay.reset(new QLineEdit(this));
  m_wgtDigitDisplay->setMaximumHeight(40);
  m_wgtDigitDisplay->setAlignment(Qt::AlignRight);
  m_wgtDigitDisplay->setReadOnly(true);

  m_wgtButtonGrid.reset(new ButtonGrid(this, m_eventSystem, m_updateLoop));

  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->addWidget(m_wgtDigitDisplay.get());
  vbox->addWidget(m_wgtButtonGrid.get());
  setLayout(vbox);

  connect(m_wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this, SLOT(onButtonClick(int)));
}

//===========================================
// CalculatorFragment::rebuild
//===========================================
void CalculatorFragment::rebuild(const CalculatorFragmentSpec& spec) {

  switch (spec.fragment) {
    case CalculatorFragmentSpec::FRAG_NORMAL_CALC:
      if (!m_normalCalcFragment) {
        m_normalCalcFragment.reset(new NormalCalculatorFragment(m_menuBar, m_actQuit, m_actAbout));
        m_normalCalcFragment->rebuild(spec.normalCalculatorFragmentSpec);
      }
      else {
        m_normalCalcFragment.reset();
      }
      break;
    case CalculatorFragmentSpec::FRAG_SHUFFLED_KEYS:
      if (!m_shuffledKeysFragment) {
        m_shuffledKeysFragment.reset(new ShuffledKeysFragment);
        m_shuffledKeysFragment->rebuild();
      }
      else {
        m_shuffledKeysFragment.reset();
      }
      break;
    default:
      EXCEPTION("Unknown fragment");
      break;
  }

  // TODO: set colour
}

//===========================================
// CalculatorFragment::onButtonClick
//===========================================
void CalculatorFragment::onButtonClick(int id) {
  static Calculator calculator;

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
        double result = calculator.equals();
/*
        if (m_appState.rootState == ST_DANGER_INFINITY && std::isinf(result)) {
          QColor origCol = palette().color(QPalette::Window);

          int frames = m_updateLoop->fps() / 2;
          m_updateLoop->add([=]() {
            static int i = -1;
            ++i;
            if (i % 2) {
              setColour(*this, origCol, QPalette::Window);
            }
            else {
              setColour(*this, Qt::white, QPalette::Window);
            }

            return i < frames;
          }, [&]() {
            transitionColour(*m_updateLoop, *this, QColor(160, 160, 160), QPalette::Window, 0.5,
              [&]() {
                m_appState.rootState = ST_SHUFFLED_KEYS;
                m_eventSystem.fire(Event("appStateUpdated"));
              });

            transitionColour(*m_updateLoop, *m_wgtDigitDisplay, QColor(160, 120, 120),
              QPalette::Base, 0.5);
          });
        }
*/
        break;
      }
      case BTN_CLEAR:
        calculator.clear();
        break;
    }
  }

  m_wgtDigitDisplay->setText(calculator.display().c_str());
}
