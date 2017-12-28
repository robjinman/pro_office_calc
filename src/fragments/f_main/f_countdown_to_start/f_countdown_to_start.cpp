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

  DBG_PRINT("FCountdownToStart::FCountdownToStart\n");
}


//===========================================
// FCountdownToStart::reload
//===========================================
void FCountdownToStart::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FCountdownToStart::reload\n");

  auto& parentData = parentFragData<FMainData>();

  m_origParentState.fnOnQuit = parentData.fnOnQuit;
  parentData.fnOnQuit = [this]() { onQuit(); };

  auto& spec = dynamic_cast<const FCountdownToStartSpec&>(spec_);
  m_stateId = spec.stateId;
}

//===========================================
// FCountdownToStart::cleanUp
//===========================================
void FCountdownToStart::cleanUp() {
  DBG_PRINT("FCountdownToStart::cleanUp\n");

  parentFragData<FMainData>().fnOnQuit = m_origParentState.fnOnQuit;
}

//===========================================
// FCountdownToStart::onQuit
//===========================================
void FCountdownToStart::onQuit() {
  if (m_stateId <= ST_NORMAL_CALCULATOR_9) {
    parentFragData<FMainData>()
      .eventSystem.fire(pEvent_t(new RequestStateChangeEvent(m_stateId + 1)));
  }
}

//===========================================
// FCountdownToStart::~FCountdownToStart
//===========================================
FCountdownToStart::~FCountdownToStart() {
  DBG_PRINT("FCountdownToStart::~FCountdownToStart\n");
}
