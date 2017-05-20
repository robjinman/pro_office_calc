#include "fragments/f_main/f_countdown_to_start/f_countdown_to_start.hpp"
#include "fragments/f_main/f_countdown_to_start/f_countdown_to_start_spec.hpp"
#include "fragments/f_main/f_main.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FCountdownToStart::FCountdownToStart
//===========================================
FCountdownToStart::FCountdownToStart(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FCountdownToStart", parent_, parentData_, m_data) {

  auto& parentData = parentFragData<FMainData>();
  parentData.fnOnQuit = [this]() { onQuit(); };
}

//===========================================
// FCountdownToStart::rebuild
//===========================================
void FCountdownToStart::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FCountdownToStartSpec&>(spec_);
  m_stateId = spec.stateId;

  Fragment::rebuild(spec_);
}

//===========================================
// FCountdownToStart::cleanUp
//===========================================
void FCountdownToStart::cleanUp() {
  parentFragData<FMainData>().fnOnQuit = []() {};
}

//===========================================
// FCountdownToStart::onQuit
//===========================================
void FCountdownToStart::onQuit() {
  DBG_PRINT("FCountdownToStart::onQuit\n");

  if (m_stateId < ST_NORMAL_CALCULATOR_10) {
    parentFragData<FMainData>().eventSystem.fire(RequestStateChangeEvent(m_stateId + 1));
  }
}
