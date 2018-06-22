#include <QPainter>
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
using millennium_bug::Coord;


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
    m_icons(icons),
    m_pixmap(32, 32) {

  QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

  setSizePolicy(sizePolicy);

  m_label = makeQtObjPtr<QLabel>();
  m_label->setSizePolicy(sizePolicy);

  m_button = makeQtObjPtr<GoodButton>();
  m_button->setSizePolicy(sizePolicy);

  m_label->setPixmap(m_pixmap);

  m_stackedLayout = makeQtObjPtr<QStackedLayout>();
  m_stackedLayout->addWidget(m_label.get());
  m_stackedLayout->addWidget(m_button.get());

  setLayout(m_stackedLayout.get());

  setHidden(true);
  setValue(0);
}

//===========================================
// MinesweeperCell::render
//===========================================
void MinesweeperCell::render() {
  QPainter painter;
  painter.begin(&m_pixmap);

  QColor bgColour(240, 240, 240);

  m_pixmap.fill(bgColour);

  int cellW = m_pixmap.width();
  int cellH = m_pixmap.height();

  if (m_value >= 1 && m_value <= 8) {
    QColor colour;
    switch (m_value) {
      case 1: {
        colour = Qt::blue;
        break;
      }
      case 2: {
        colour = QColor(0, 128, 0);
        break;
      }
      case 3: {
        colour = Qt::red;
        break;
      }
      case 4: {
        colour = Qt::darkBlue;
        break;
      }
      case 5: {
        colour = QColor(178, 34, 34);
        break;
      }
      case 6: {
        colour = Qt::darkCyan;
        break;
      }
      case 7: {
        colour = Qt::magenta;
        break;
      }
      case 8: {
        colour = Qt::black;
        break;
      }
    }

    int w = 10; // TODO
    int h = 14;

    QFont font;
    font.setPixelSize(h);

    painter.setFont(font);
    painter.setPen(colour);
    painter.drawText(0.5 * (cellW - w), 0.5 * (cellH + h), std::to_string(m_value).c_str());
  }
  else if (m_value == MINE) {
    int w = 18;
    int h = 18;

    m_icons.mine.paint(&painter, 0.5 * (cellW - w), 0.5 * (cellH - h), w, h);
  }

  if (m_hasPlayer) {
    int w = 18;
    int h = 18;

    m_icons.player.paint(&painter, 0.5 * (cellW - w), 0.5 * (cellH - h), w, h);
  }

  painter.end();

  m_label->setPixmap(m_pixmap);
}

//===========================================
// MinesweeperCell::setValue
//===========================================
void MinesweeperCell::setValue(int val) {
  m_value = val;
  render();
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
// MinesweeperCell::onPlayerChangeCell
//===========================================
void MinesweeperCell::onPlayerChangeCell(int row, int col) {
  if (row == this->row && col == this->col) {
    m_hasPlayer = true;
    setHidden(false);
  }
  else {
    m_hasPlayer = false;
  }

  render();
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
  m_icons.mine = QIcon(config::dataPath("millennium_bug/mine.png").c_str());
  m_icons.noMine = QIcon(config::dataPath("millennium_bug/no_mine.png").c_str());
  m_icons.player = QIcon(config::dataPath("millennium_bug/player.png").c_str());

  set<Coord> coords = placeMines();
  setNumbers();

  commonData.eventSystem.listen("millenniumBug/innerCellEntered",
    std::bind(&FMinesweeper::onInnerCellEntered, this, std::placeholders::_1),
    m_innerCellEnteredIdx);

  commonData.eventSystem.fire(pEvent_t(new millennium_bug::MinesweeperSetupEvent(coords)));
}

//===========================================
// FMinesweeper::onInnerCellEntered
//===========================================
void FMinesweeper::onInnerCellEntered(const Event& e_) {
  auto& e = dynamic_cast<const millennium_bug::InnerCellEnteredEvent&>(e_);

  DBG_PRINT("Player has entered cell " << e.coords.row << ", " << e.coords.col << "\n");

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      m_cells[i][j]->onPlayerChangeCell(e.coords.row, e.coords.col);
    }
  }
}

//===========================================
// FMinesweeper::placeMines
//===========================================
set<Coord> FMinesweeper::placeMines() {
  vector<Coord> coords;
  set<Coord> mineCoords;

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      if (i == 0 && j == 0) {
        continue;
      }

      coords.push_back(Coord{i, j});
    }
  }

  std::shuffle(coords.begin(), coords.end(), randEngine);

  for (int i = 0; i < MINES; ++i) {
    int row = coords[i].row;
    int col = coords[i].col;

    m_cells[row][col]->setValue(MINE);

    mineCoords.insert(coords[i]);
  }

  return mineCoords;
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
// FMinesweeper::clearNeighbours_r
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

  commonData.eventSystem.forget(m_innerCellEnteredIdx);
}

//===========================================
// FMinesweeper::~FMinesweeper
//===========================================
FMinesweeper::~FMinesweeper() {
  DBG_PRINT("FMinesweeper::~FMinesweeper\n");
}
