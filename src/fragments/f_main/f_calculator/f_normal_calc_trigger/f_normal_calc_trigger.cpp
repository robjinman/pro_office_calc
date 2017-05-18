#include <QMainWindow>
#include "fragments/f_main/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger.hpp"
#include "fragments/f_main/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger_spec.hpp"
#include "fragments/f_main/f_calculator/f_calculator.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "event_system.hpp"
#include "update_loop.hpp"
#include "effects.hpp"


//===========================================
// FNormalCalcTrigger::FNormalCalcTrigger
//===========================================
FNormalCalcTrigger::FNormalCalcTrigger(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FNormalCalcTrigger", parent_, parentData_, m_data),
    m_stateId(0) {

  auto& parentData = parentFragData<FCalculatorData>();
  auto& parent = parentFrag<FCalculator>();

  disconnect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), &parent,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));
}

//===========================================
// FNormalCalcTrigger::rebuild
//===========================================
void FNormalCalcTrigger::rebuild(const FragmentSpec& spec) {
  Fragment::rebuild(spec);
  m_stateId = dynamic_cast<const FNormalCalcTriggerSpec&>(spec).stateId;
}

//===========================================
// FNormalCalcTrigger::cleanUp
//===========================================
void FNormalCalcTrigger::cleanUp() {
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

  if (m_stateId == ST_NORMAL_CALCULATOR_10 && id == BTN_EQUALS) {
    double result = data.calculator.equals();
    data.wgtDigitDisplay->setText(data.calculator.display().c_str());

    if (std::isinf(result)) {
      QColor origCol = data.window->palette().color(QPalette::Window);

      int frames = data.updateLoop->fps() / 2;
      int i = 0;
      data.updateLoop->add([=,&data]() mutable {
        ++i;
        if (i % 2) {
          setColour(*data.window, origCol, QPalette::Window);
        }
        else {
          setColour(*data.window, Qt::white, QPalette::Window);
        }

        return i < frames;
      }, [&]() {
        transitionColour(*data.updateLoop, *data.window, QColor(160, 160, 160), QPalette::Window,
          0.5, [&]() {

          data.eventSystem->fire(RequestStateChangeEvent(ST_SHUFFLED_KEYS));
        });

        transitionColour(*data.updateLoop, *data.wgtDigitDisplay, QColor(160, 120, 120),
          QPalette::Base, 0.5);
      });
    }
  }
  else {
    parentFrag<FCalculator>().onButtonClick(id);
  }
}
