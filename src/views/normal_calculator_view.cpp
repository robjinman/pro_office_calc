#include "normal_calculator_view.hpp"
#include "event_system.hpp"
#include "new_root_state_event.hpp"
#include "exception.hpp"
#include "state_ids.hpp"


NormalCalculatorView(EventSystem& eventSystem)
  : m_eventSystem(eventSystem) {}

void NormalCalculatorView::setup(int rootState) {
  if (!ltelte<int>(ST_NORMAL_CALCULATOR_0, m_appState.rootState, ST_NORMAL_CALCULATOR_10)) {
    EXCEPTION();
  }

  if (rootState < ST_NORMAL_CALCULATOR_10) {
    m_eventSystem.fire(NewRootStateEvent(rootState + 1));
  }
  else {
    m_eventSystem.fire(NewRootStateEvent(ST_DANGER_INFINITY));
  }
}
