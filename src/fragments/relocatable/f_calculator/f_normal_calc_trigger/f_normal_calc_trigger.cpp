#include <cassert>
#include <QMainWindow>
#include <QPushButton>
#include <QApplication>
#include "fragments/relocatable/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger.hpp"
#include "fragments/relocatable/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger_spec.hpp"
#include "fragments/relocatable/f_calculator/f_calculator.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "event_system.hpp"
#include "update_loop.hpp"
#include "effects.hpp"
#include "utils.hpp"


//===========================================
// getMainWindow
//===========================================
static QMainWindow* getMainWindow() {
  foreach(QWidget* widget, qApp->topLevelWidgets()) {
    if (QMainWindow* mainWindow = qobject_cast<QMainWindow*>(widget)) {
      return mainWindow;
    }
  }

  return nullptr;
}

//===========================================
// FNormalCalcTrigger::FNormalCalcTrigger
//===========================================
FNormalCalcTrigger::FNormalCalcTrigger(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FNormalCalcTrigger", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FNormalCalcTrigger::FNormalCalcTrigger\n");
}

//===========================================
// FNormalCalcTrigger::reload
//===========================================
void FNormalCalcTrigger::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FNormalCalcTrigger::reload\n");

  auto& parentData = parentFragData<FCalculatorData>();

  disconnect(parentData.wgtCalculator->wgtButtonGrid.get(), SIGNAL(buttonClicked(int)),
    parentData.wgtCalculator.get(), SLOT(onButtonClick(int)));
  connect(parentData.wgtCalculator->wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));

  auto& spec = dynamic_cast<const FNormalCalcTriggerSpec&>(spec_);

  m_targetWindowColour = spec.targetWindowColour;
  m_targetDisplayColour = spec.targetDisplayColour;
  m_symbols = spec.symbols;
}

//===========================================
// FNormalCalcTrigger::cleanUp
//===========================================
void FNormalCalcTrigger::cleanUp() {
  DBG_PRINT("FNormalCalcTrigger::cleanUp\n");

  auto& parentData = parentFragData<FCalculatorData>();

  disconnect(parentData.wgtCalculator->wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtCalculator->wgtButtonGrid.get(), SIGNAL(buttonClicked(int)),
    parentData.wgtCalculator.get(), SLOT(onButtonClick(int)));
}

//===========================================
// FNormalCalcTrigger:onButtonClick
//===========================================
void FNormalCalcTrigger::onButtonClick(int id) {
  auto& data = parentFragData<FCalculatorData>();

  QWidget* window = getMainWindow();
  assert(window != nullptr);

  if (id == BTN_EQUALS) {
    double result = data.wgtCalculator->calculator.equals();
    data.wgtCalculator->wgtDigitDisplay->setText(data.wgtCalculator->calculator.display().c_str());

    if (std::isinf(result)) {
      QColor origCol = window->palette().color(QPalette::Window);
      int i = 0;

      commonData.updateLoop.add([=,&data]() mutable {
        auto& buttons = data.wgtCalculator->wgtButtonGrid->buttons;
        ++i;

        int j = 0;
        for (auto it = buttons.begin(); it != buttons.end(); ++it) {
          QChar ch = m_symbols[(i + j) % m_symbols.length()];
          (*it)->setText(ch);
          ++j;
        }

        return i < commonData.updateLoop.fps() * 1.5;
      });

      commonData.updateLoop.add([=,&data]() mutable {
        ++i;
        if (i % 2) {
          setColour(*window, origCol, QPalette::Window);
        }
        else {
          setColour(*window, Qt::white, QPalette::Window);
        }

        return i < commonData.updateLoop.fps();
      }, [&, window]() {
        transitionColour(commonData.updateLoop, *window, m_targetWindowColour,
          QPalette::Window, 0.5, [&]() {

          commonData.eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_SHUFFLED_KEYS)));
        });

        transitionColour(commonData.updateLoop, *data.wgtCalculator->wgtDigitDisplay,
          m_targetDisplayColour, QPalette::Base, 0.5);
      });
    }
  }
  else {
    data.wgtCalculator->onButtonClick(id);
  }
}

//===========================================
// FNormalCalcTrigger::~FNormalCalcTrigger
//===========================================
FNormalCalcTrigger::~FNormalCalcTrigger() {
  DBG_PRINT("FNormalCalcTrigger::~FNormalCalcTrigger\n");
}
