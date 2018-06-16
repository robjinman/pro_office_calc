#include <random>
#include <vector>
#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/f_minesweeper.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/f_minesweeper_spec.hpp"
#include "utils.hpp"
#include "event_system.hpp"
#include "app_config.hpp"


using std::set;
using std::vector;


static const int ROWS = 8;
static const int COLS = 8;
static const int MINES = 20;

// Value used to represent a mine
static const int MINE = 10;

static std::mt19937 randEngine(randomSeed());


//===========================================
// MinesweeperCell::MinesweeperCell
//===========================================
MinesweeperCell::MinesweeperCell(int row, int col, const IconSet& icons)
  : QWidget(),
    row(row),
    col(col),
    m_icons(icons) {

  QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

  setSizePolicy(sizePolicy);

  m_label = makeQtObjPtr<QLabel>();
  m_label->setSizePolicy(sizePolicy);

  m_button = makeQtObjPtr<GoodButton>();
  m_button->setSizePolicy(sizePolicy);

  m_stackedLayout = makeQtObjPtr<QStackedLayout>();
  m_stackedLayout->addWidget(m_label.get());
  m_stackedLayout->addWidget(m_button.get());

  setLayout(m_stackedLayout.get());

  setHidden(true);
  setValue(0);
}

//===========================================
// MinesweeperCell::setValue
//===========================================
void MinesweeperCell::setValue(int val) {
  m_value = val;
  m_label->setText(std::to_string(val).c_str());
}

//===========================================
// MinesweeperCell::setFlagged
//===========================================
void MinesweeperCell::setFlagged(bool flagged) {
  m_flagged = flagged;

  if (m_flagged) {
    m_button->setIcon(m_icons.flag);
  }
  else {
    m_button->setIcon(QIcon());
  }
}

//===========================================
// MinesweeperCell::hidden
//===========================================
bool MinesweeperCell::hidden() const {
  return m_stackedLayout->currentIndex() == 1;
}

//===========================================
// MinesweeperCell::setHidden
//===========================================
void MinesweeperCell::setHidden(bool hidden) {
  m_stackedLayout->setCurrentIndex(hidden ? 1 : 0);
}

//===========================================
// FMinesweeper::FMinesweeper
//===========================================
FMinesweeper::FMinesweeper(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FMinesweeper", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FMinesweeper::FMinesweeper\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_grid = makeQtObjPtr<QGridLayout>();
  m_grid->setSpacing(0);
  m_grid->setContentsMargins(0, 0, 0, 0);

  m_buttonGroup = makeQtObjPtr<GoodButtonGroup>();

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      m_cells[i][j] = makeQtObjPtr<MinesweeperCell>(i, j, m_icons);
      MinesweeperCell* cell = m_cells[i][j].get();

      m_buttonGroup->addGoodButton(&cell->button(), i * COLS + j);
      m_grid->addWidget(cell, i, j);
    }
  }

  setLayout(m_grid.get());

  parentData.box->addWidget(this);

  connect(m_buttonGroup.get(), SIGNAL(buttonClicked(int)), this, SLOT(onBtnClick(int)));
  connect(m_buttonGroup.get(), SIGNAL(rightClicked(int)), this, SLOT(onBtnRightClick(int)));

  m_icons.flag = QIcon(config::dataPath("millennium_bug/flag.png").c_str());

  placeMines();
  setNumbers();
}

//===========================================
// FMinesweeper::placeMines
//===========================================
void FMinesweeper::placeMines() {
  struct Coord {
    int row;
    int col;
  };

  vector<Coord> coords;

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      coords.push_back(Coord{i, j});
    }
  }

  std::shuffle(coords.begin(), coords.end(), randEngine);

  for (int i = 0; i < MINES; ++i) {
    int row = coords[i].row;
    int col = coords[i].col;

    m_cells[row][col]->setValue(MINE);
  }
}

//===========================================
// FMinesweeper::getNeighbours
//===========================================
set<MinesweeperCell*> FMinesweeper::getNeighbours(const MinesweeperCell& cell) const {
  set<MinesweeperCell*> neighbours;

  for (int i = cell.row - 1; i <= cell.row + 1; ++i) {
    for (int j = cell.col - 1; j <= cell.col + 1; ++j) {
      if (i == cell.row && j == cell.col) {
        continue;
      }

      if (i < 0 || i >= ROWS) {
        continue;
      }

      if (j < 0 || j >= COLS) {
        continue;
      }

      neighbours.insert(m_cells[i][j].get());
    }
  }

  return neighbours;
}

//===========================================
// FMinesweeper::setNumbers
//===========================================
void FMinesweeper::setNumbers() {
  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      MinesweeperCell& cell = *m_cells[i][j];

      if (cell.value() == MINE) {
        set<MinesweeperCell*> neighbours = getNeighbours(cell);

        for (auto neighbour : neighbours) {
          if (neighbour->value() != MINE) {
            neighbour->setValue(neighbour->value() + 1);
          }
        }
      }
    }
  }
}

//===========================================
// FMinesweeper::clearNeighbours
//===========================================
void FMinesweeper::clearNeighbours_r(const MinesweeperCell& cell,
  set<const MinesweeperCell*>& visited) {

  if (cell.value() == 0) {
    auto neighbours = getNeighbours(cell);
    for (MinesweeperCell* neighbour : neighbours) {

      if (visited.count(neighbour) == 0 && neighbour->hidden() && !neighbour->flagged()) {
        neighbour->setHidden(false);
        visited.insert(neighbour);

        clearNeighbours_r(*neighbour, visited);
      }
    }
  }
}

//===========================================
// FMinesweeper::onBtnClick
//===========================================
void FMinesweeper::onBtnClick(int id) {
  int row = id / COLS;
  int col = id % COLS;

  DBG_PRINT("Left clicked button " << row << ", " << col << "\n");

  MinesweeperCell& cell = *m_cells[row][col];

  if (cell.hidden() && !cell.flagged()) {
    switch (cell.value()) {
      case 0: {
        set<const MinesweeperCell*> visited{};
        clearNeighbours_r(cell, visited);
        break;
      }
      case MINE: {
        DBG_PRINT("Boom!\n");
        // TODO
        break;
      }
      default: {
        // Do nothing
        break;
      }
    }

    cell.setHidden(false);
  }
}

//===========================================
// FMinesweeper::onBtnRightClick
//===========================================
void FMinesweeper::onBtnRightClick(int id) {
  int row = id / COLS;
  int col = id % COLS;

  DBG_PRINT("Right clicked button " << row << ", " << col << "\n");

  auto& cell = m_cells[row][col];

  if (cell->hidden()) {
    cell->setFlagged(!cell->flagged());
  }
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
