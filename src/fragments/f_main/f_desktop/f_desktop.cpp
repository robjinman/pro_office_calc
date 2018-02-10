#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_desktop/f_desktop.hpp"
#include "fragments/f_main/f_desktop/f_desktop_spec.hpp"
#include "utils.hpp"


//===========================================
// FDesktop::FDesktop
//===========================================
FDesktop::FDesktop(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FDesktop", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FDesktop::FDesktop\n");
}

//===========================================
// FDesktop::reload
//===========================================
void FDesktop::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FDesktop::reload\n");

}

//===========================================
// FDesktop::cleanUp
//===========================================
void FDesktop::cleanUp() {
  DBG_PRINT("FDesktop::cleanUp\n");

}

//===========================================
// FDesktop::~FDesktop
//===========================================
FDesktop::~FDesktop() {
  DBG_PRINT("FDesktop::~FDesktop\n");

}
