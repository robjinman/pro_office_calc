#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_tetrominos/f_tetrominos.hpp"
#include "fragments/f_main/f_tetrominos/f_tetrominos_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FTetrominos::FTetrominos
//===========================================
FTetrominos::FTetrominos(Fragment& parent_, FragmentData& parentData_)
  : QLabel(nullptr),
    Fragment("FTetrominos", parent_, parentData_, m_data) {

  DBG_PRINT("FTetrominos::FTetrominos\n");

  auto& parentData = dynamic_cast<FMainData&>(parentData_);
  m_data.eventSystem = &parentData.eventSystem;
}

//===========================================
// FTetrominos::reload
//===========================================
void FTetrominos::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FTetrominos::reload\n");
}

//===========================================
// FTetrominos::cleanUp
//===========================================
void FTetrominos::cleanUp() {
  DBG_PRINT("FTetrominos::cleanUp\n");
}

//===========================================
// FTetrominos::~FTetrominos
//===========================================
FTetrominos::~FTetrominos() {
  DBG_PRINT("FTetrominos::~FTetrominos\n");
}
