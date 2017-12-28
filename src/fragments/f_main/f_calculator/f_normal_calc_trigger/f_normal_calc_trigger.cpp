#include <QMainWindow>
#include <QPushButton>
#include "fragments/f_main/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger.hpp"
#include "fragments/f_main/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger_spec.hpp"
#include "fragments/f_main/f_calculator/f_calculator.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "event_system.hpp"
#include "update_loop.hpp"
#include "effects.hpp"
#include "utils.hpp"


//===========================================
// FNormalCalcTrigger::FNormalCalcTrigger
//===========================================
FNormalCalcTrigger::FNormalCalcTrigger(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FNormalCalcTrigger", parent_, parentData_, m_data) {

  DBG_PRINT("FNormalCalcTrigger::FNormalCalcTrigger\n");
}

//===========================================
// FNormalCalcTrigger::initialise
//===========================================
void FNormalCalcTrigger::initialise(const FragmentSpec& spec_) {
  DBG_PRINT("FNormalCalcTrigger::initialise\n");
}

//===========================================
// FNormalCalcTrigger::reload
//===========================================
void FNormalCalcTrigger::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FNormalCalcTrigger::reload\n");

  auto& parentData = parentFragData<FCalculatorData>();
  auto& parent = parentFrag<FCalculator>();

  disconnect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), &parent,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
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
  auto& parent = parentFrag<FCalculator>();

  disconnect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), &parent,
    SLOT(onButtonClick(int)));
}

//===========================================
// FNormalCalcTrigger:onButtonClick
//===========================================
void FNormalCalcTrigger::onButtonClick(int id) {
  auto& data = parentFragData<FCalculatorData>();

  if (id == BTN_EQUALS) {
    double result = data.calculator.equals();
    data.wgtDigitDisplay->setText(data.calculator.display().c_str());

    if (std::isinf(result)) {
      QColor origCol = data.window->palette().color(QPalette::Window);
      int i = 0;

      data.updateLoop->add([=,&data]() mutable {
        auto& buttons = data.wgtButtonGrid->buttons;
        ++i;

        int j = 0;
        for (auto it = buttons.begin(); it != buttons.end(); ++it) {
          QChar ch = m_symbols[(i + j) % m_symbols.length()];
          (*it)->setText(ch);
          ++j;
        }

        return i < data.updateLoop->fps() * 1.5;
      });

      data.updateLoop->add([=,&data]() mutable {
        ++i;
        if (i % 2) {
          setColour(*data.window, origCol, QPalette::Window);
        }
        else {
          setColour(*data.window, Qt::white, QPalette::Window);
        }

        return i < data.updateLoop->fps();
      }, [&]() {
        transitionColour(*data.updateLoop, *data.window, m_targetWindowColour, QPalette::Window,
          0.5, [&]() {

          data.eventSystem->fire(pEvent_t(new RequestStateChangeEvent(ST_SHUFFLED_KEYS)));
        });

        transitionColour(*data.updateLoop, *data.wgtDigitDisplay, m_targetDisplayColour,
          QPalette::Base, 0.5);
      });
    }
  }
  else {
    parentFrag<FCalculator>().onButtonClick(id);
  }
}

//===========================================
// FNormalCalcTrigger::~FNormalCalcTrigger
//===========================================
FNormalCalcTrigger::~FNormalCalcTrigger() {
  DBG_PRINT("FNormalCalcTrigger::~FNormalCalcTrigger\n");
}
