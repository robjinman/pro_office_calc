#include <vector>
#include <array>
#include <random>
#include <QPainter>
#include "fragments/f_main/f_tetrominos/f_tetrominos.hpp"
#include "fragments/f_main/f_tetrominos/f_tetrominos_spec.hpp"
#include "utils.hpp"


using std::array;
using std::vector;


const int BLOCK_SIZE = 5;
const double FRAME_RATE = 15.0;
const double AVERAGE_ANGULAR_SPEED = 100.0; // Degrees per second
const double AVERAGE_SPEED = 35.0; // Pixels per second
const double SPEED_STD_DEVIATION = 7; // Average deviation from the average


static std::random_device rd;
static std::mt19937 randEngine(rd());


//===========================================
// makeTetromino
//===========================================
static void makeTetromino(const array<array<int, 4>, 4>& matrix, array<QPolygon, 4>& blocks) {
  const int W = BLOCK_SIZE;
  int idx = 0;

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (matrix[i][j]) {
        QPolygon poly;

        poly << QPoint(0, 0)
             << QPoint(W, 0)
             << QPoint(W, W)
             << QPoint(0, W);

        QTransform t = QTransform().translate(i * W, j * W);

        blocks[idx++] = t.map(poly);
      }
    }
  }
}

//===========================================
// FTetrominos::FTetrominos
//===========================================
FTetrominos::FTetrominos(Fragment& parent_, FragmentData& parentData_)
  : QLabel(nullptr),
    Fragment("FTetrominos", parent_, parentData_, m_data) {

  DBG_PRINT("FTetrominos::FTetrominos\n");

  auto& parent = dynamic_cast<QWidget&>(parent_);

  setParent(&parent);

  setScaledContents(true);
  setAttribute(Qt::WA_TransparentForMouseEvents);

  m_data.timer.reset(new QTimer(this));

  connect(m_data.timer.get(), SIGNAL(timeout()), this, SLOT(tick()));

  constructTetrominos();



  show();
}

//===========================================
// FTetrominos::constructTetrominos
//===========================================
void FTetrominos::constructTetrominos() {
  auto& parent = parentFrag<QWidget>();

  double winW = parent.size().width();
  double winH = parent.size().height();

  int spacing = 10;
  int tetroSz = BLOCK_SIZE * 4 + spacing;
  int rows = winH / tetroSz;
  int cols = winW / tetroSz;

  std::normal_distribution<double> randSpeed(AVERAGE_SPEED / FRAME_RATE,
    SPEED_STD_DEVIATION / FRAME_RATE);
  std::normal_distribution<double> randAngle(0, AVERAGE_ANGULAR_SPEED / FRAME_RATE);

  for (int i = 0; i < cols; ++i) {
    double dy = randSpeed(randEngine);

    for (int j = 0; j < rows; ++j) {
      if (rand() % 4 != 0) {
        continue;
      }

      auto kind = static_cast<Tetromino::kind_t>(rand() % 7);
      double da = randAngle(randEngine);

      Tetromino t{kind, (i + 0.5) * tetroSz, winH - j * tetroSz, 0, dy, da};

      switch (kind) {
        case Tetromino::I:
          t.colour = QColor(230, 50, 50);
          makeTetromino({{
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0}
          }}, t.blocks);
          break;
        case Tetromino::J:
          t.colour = QColor(250, 40, 200);
          makeTetromino({{
            {0, 0, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 1, 1, 0}
          }}, t.blocks);
          break;
        case Tetromino::L:
          t.colour = QColor(250, 250, 0);
          makeTetromino({{
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0}
          }}, t.blocks);
          break;
        case Tetromino::O:
          t.colour = QColor(100, 220, 250);
          makeTetromino({{
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
          }}, t.blocks);
          break;
        case Tetromino::S:
          t.colour = QColor(20, 20, 200);
          makeTetromino({{
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
          }}, t.blocks);
          break;
        case Tetromino::T:
          t.colour = QColor(160, 160, 160);
          makeTetromino({{
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
          }}, t.blocks);
          break;
        case Tetromino::Z:
          t.colour = QColor(50, 250, 50);
          makeTetromino({{
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 1},
            {0, 0, 0, 0}
          }}, t.blocks);
          break;
      }

      m_tetrominos.push_back(t);
    }
  }
}

//===========================================
// FTetrominos::reload
//===========================================
void FTetrominos::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FTetrominos::reload\n");

  auto& parent = parentFrag<QWidget>();

  move(0, 0);
  resize(parent.size());

  m_buffer.reset(new QImage(parent.size(), QImage::Format_ARGB32));

  m_data.timer->start(1000.0 / FRAME_RATE);
}

//===========================================
// FTetrominos::drawTetrominos
//===========================================
void FTetrominos::drawTetrominos(QImage& buffer) {
  QPainter painter;
  painter.begin(&buffer);

  buffer.fill(Qt::GlobalColor::transparent);

  for (auto it = m_tetrominos.begin(); it != m_tetrominos.end(); ++it) {
    const Tetromino& t = *it;

    int offset = -BLOCK_SIZE * 2.0;
    QTransform transform = QTransform().translate(t.x, t.y).rotate(t.a).translate(offset, offset);

    painter.setBrush(t.colour);

    for (int i = 0; i < 4; ++i) {
      painter.drawPolygon(transform.map(t.blocks[i]));
    }
  };

  painter.end();
}

//===========================================
// FTetrominos::moveTetrominos
//===========================================
void FTetrominos::moveTetrominos() {
  for (auto it = m_tetrominos.begin(); it != m_tetrominos.end(); ++it) {
    Tetromino& t = *it;

    t.y += t.dy;
    t.a += t.da;

    auto& parent = parentFrag<QWidget>();

    if (t.y > parent.size().height() + BLOCK_SIZE) {
      t.y = -BLOCK_SIZE;
    }
  }
}

//===========================================
// FTetrominos::tick
//===========================================
void FTetrominos::tick() {
  moveTetrominos();
  drawTetrominos(*m_buffer);

  setPixmap(QPixmap::fromImage(*m_buffer));

  raise();
}

//===========================================
// FTetrominos::cleanUp
//===========================================
void FTetrominos::cleanUp() {
  DBG_PRINT("FTetrominos::cleanUp\n");

  setParent(nullptr);
}

//===========================================
// FTetrominos::~FTetrominos
//===========================================
FTetrominos::~FTetrominos() {
  DBG_PRINT("FTetrominos::~FTetrominos\n");
}
