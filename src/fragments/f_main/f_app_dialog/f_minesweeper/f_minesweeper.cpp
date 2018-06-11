#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/f_minesweeper.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/f_minesweeper_spec.hpp"
#include "utils.hpp"
#include "event_system.hpp"
#include "app_config.hpp"


static const int ROWS = 10;
static const int COLS = 10;


//===========================================
// FMinesweeper::FMinesweeper
//===========================================
FMinesweeper::FMinesweeper(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FMinesweeper", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FMinesweeper::FMinesweeper\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_data.grid = makeQtObjPtr<QGridLayout>();
  m_data.grid->setSpacing(0);
  m_data.grid->setContentsMargins(0, 0, 0, 0);

  m_data.buttonGroup = makeQtObjPtr<QButtonGroup>();

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      m_data.buttons[i][j] = makeQtObjPtr<QPushButton>();
      QPushButton* button = m_data.buttons[i][j].get();

      m_data.buttonGroup->addButton(button, i * COLS + j);
      m_data.grid->addWidget(button, i, j);
    }
  }

  setLayout(m_data.grid.get());

  parentData.box->addWidget(this);

  connect(m_data.buttonGroup.get(), SIGNAL(buttonClicked(int)), this, SLOT(onBtnClick(int)));
}

//===========================================
// FMinesweeper::onBtnClick
//===========================================
void FMinesweeper::onBtnClick(int id) {
  DBG_PRINT("Button " << id << " clicked\n");
}

//===========================================
// FMinesweeper::reload
//===========================================
void FMinesweeper::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FMinesweeper::reload\n");

}

//===========================================
// FMinesweeper::cleanUp
//===========================================
void FMinesweeper::cleanUp() {
  DBG_PRINT("FMinesweeper::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();
  parentData.box->removeWidget(this);
}

//===========================================
// FMinesweeper::~FMinesweeper
//===========================================
FMinesweeper::~FMinesweeper() {
  DBG_PRINT("FMinesweeper::~FMinesweeper\n");
}
