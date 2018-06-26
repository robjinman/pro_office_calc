#include <QSpacerItem>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_desktop/f_desktop.hpp"
#include "fragments/f_main/f_desktop/f_desktop_spec.hpp"
#include "utils.hpp"
#include "event_system.hpp"


using std::string;


const int ICON_WIDTH = 40;
const int ICON_HEIGHT = 40;


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

  auto& spec = dynamic_cast<const FDesktopSpec&>(spec_);
  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  delete m_data.grid.release();
  m_data.grid = makeQtObjPtr<QGridLayout>();
  setLayout(m_data.grid.get());

  int cols = 6;
  int rows = 5;

  for (int i = 0; i < cols; ++i) {
    for (int j = 0; j < rows; ++j) {
      QSpacerItem* spacer = new QSpacerItem(ICON_WIDTH, ICON_HEIGHT);

      // Grid takes ownership
      m_data.grid->addItem(spacer, j, i);
    }
  }

  int col = 0;
  int row = 0;
  int i = 0;

  m_data.icons.clear();

  for (auto& icon : spec.icons) {
    auto wgtIcon = makeQtObjPtr<DesktopIcon>(icon.eventName, icon.image, icon.text);

    connect(wgtIcon.get(), SIGNAL(activated(const std::string&)), this,
      SLOT(onIconActivate(const std::string&)));

    m_data.grid->addWidget(wgtIcon.get(), row, col);

    m_data.icons.push_back(std::move(wgtIcon));

    row = (row + 1) % rows;
    col = i / rows;
    ++i;
  }
}

//===========================================
// FDesktop::onIconActivate
//===========================================
void FDesktop::onIconActivate(const string& name) {
  pEvent_t event(new Event(name));

  commonData.eventSystem.fire(std::move(event));
}

//===========================================
// FDesktop::cleanUp
//===========================================
void FDesktop::cleanUp() {
  DBG_PRINT("FDesktop::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);
}

//===========================================
// FDesktop::~FDesktop
//===========================================
FDesktop::~FDesktop() {
  DBG_PRINT("FDesktop::~FDesktop\n");

}
