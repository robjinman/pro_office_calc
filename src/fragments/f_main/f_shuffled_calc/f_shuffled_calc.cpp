#include <algorithm>
#include <cassert>
#include <QPushButton>
#include <QButtonGroup>
#include <QPainter>
#include <vector>
#include "event_system.hpp"
#include "update_loop.hpp"
#include "effects.hpp"
#include "strings.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "utils.hpp"
#include "effects.hpp"
#include "fragments/f_main/f_shuffled_calc/f_shuffled_calc.hpp"
#include "fragments/f_main/f_shuffled_calc/f_shuffled_calc_spec.hpp"
#include "fragments/f_main/f_main.hpp"


using std::random_shuffle;


//===========================================
// idToChar
//===========================================
static QChar idToChar(int id) {
  if (id < 10) {
    return QString::number(id)[0];
  }
  switch (id) {
    case BTN_PLUS: return '+';
    case BTN_MINUS: return '-';
    case BTN_TIMES: return '*';
    case BTN_DIVIDE: return '/';
    case BTN_POINT: return '.';
    case BTN_CLEAR: return 'C';
    case BTN_EQUALS: return '=';
  }
  return '_';
}

//===========================================
// FShuffledCalc::FShuffledCalc
//===========================================
FShuffledCalc::FShuffledCalc(Fragment& parent_, FragmentData& parentData_)
  : QWidget(nullptr),
    Fragment("FShuffledCalc", parent_, parentData_, m_data) {}

//===========================================
// FShuffledCalc::rebuild
//===========================================
void FShuffledCalc::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<FMain>();
  auto& parentData = parentFragData<FMainData>();

  m_eventSystem = &parentData.eventSystem;
  m_updateLoop = &parentData.updateLoop;

  parent.setCentralWidget(this);
  QWidget::setParent(&parent);
  setGeometry(parent.geometry());

  m_wgtDigitDisplay.reset(new QLineEdit(this));
  m_wgtDigitDisplay->setMaximumHeight(40);
  m_wgtDigitDisplay->setAlignment(Qt::AlignRight);
  m_wgtDigitDisplay->setReadOnly(true);

  m_wgtButtonGrid.reset(new ButtonGrid(this));

  m_glitchOverlay.reset(new QLabel(this));
  m_glitchBuffer.reset(new QImage(size(), QImage::Format_ARGB32));
  m_glitchTimer.reset(new QTimer(this));
  m_glitchOverlay->setGeometry(rect());
  m_glitchOverlay->setScaledContents(true);
  m_glitchOverlay->show();

  m_vbox.reset(new QVBoxLayout);
  m_vbox->addWidget(m_wgtDigitDisplay.get());
  m_vbox->addWidget(m_wgtButtonGrid.get());
  setLayout(m_vbox.get());

  connect(m_glitchTimer.get(), SIGNAL(timeout()), this, SLOT(tick()));
  connect(m_wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this, SLOT(onButtonClick(int)));

  m_glitchTimer->start(1000);

  m_origParentState.centralWidget = parent.centralWidget();

  auto& spec = dynamic_cast<const FShuffledCalcSpec&>(spec_);
  m_targetValue = spec.targetValue;

  ucs4string_t symbols_ = utf8ToUcs4(spec.symbols.toStdString());
  random_shuffle(symbols_.begin(), symbols_.end());
  QString symbols(ucs4ToUtf8(symbols_).c_str());

  auto& buttons = m_wgtButtonGrid->buttons;
  auto& group = m_wgtButtonGrid->buttonGroup;

  assert(symbols.length() == static_cast<int>(buttons.size()));

  std::vector<int> ids({
    BTN_ZERO,
    BTN_ONE,
    BTN_TWO,
    BTN_THREE,
    BTN_FOUR,
    BTN_FIVE,
    BTN_SIX,
    BTN_SEVEN,
    BTN_EIGHT,
    BTN_NINE,
    BTN_PLUS,
    BTN_MINUS,
    BTN_TIMES,
    BTN_DIVIDE,
    BTN_POINT,
    BTN_CLEAR,
    BTN_EQUALS
  });

  std::vector<int> newIds = ids;
  random_shuffle(newIds.begin(), newIds.end());

  for (int i = 0; i < symbols.length(); ++i) {
    group->setId(buttons[i].get(), newIds[i]);
    QChar ch = symbols[i];
    buttons[i]->setText(ch);
    m_symbols[idToChar(newIds[i])] = ch;
  }

  DBG_PRINT("Answer = " << translateToSymbols(m_targetValue.c_str()).toStdString() << "\n");

  setColour(*m_wgtDigitDisplay, spec.displayColour, QPalette::Base);

  Fragment::rebuild(spec_);
}

//===========================================
// FShuffledCalc::cleanUp
//===========================================
void FShuffledCalc::cleanUp() {
  auto& parent = parentFrag<FMain>();

  parent.setCentralWidget(m_origParentState.centralWidget);
}

//===========================================
// FShuffledCalc::translateToSymbols
//===========================================
QString FShuffledCalc::translateToSymbols(const QString& str) const {
  QString result;

  for (int i = 0; i < str.length(); ++i) {
    auto it = m_symbols.find(str[i]);
    result += (it == m_symbols.end() ? str[i] : it->second);
  }

  return result;
}

//===========================================
// FShuffledCalc::tick
//===========================================
void FShuffledCalc::tick() {
  if (!m_glitchOverlay->isVisible()) {
    QImage buf(m_glitchBuffer->size(), m_glitchBuffer->format());

    render(&buf);
    garbleImage(buf, *m_glitchBuffer);

    m_glitchOverlay->setPixmap(QPixmap::fromImage(*m_glitchBuffer));
    m_glitchOverlay->setVisible(true);

    m_glitchTimer->setInterval(100);
  }
  else {
    m_glitchOverlay->setVisible(false);

    std::uniform_int_distribution<int> dist(100, 2000);
    m_glitchTimer->setInterval(dist(m_randEngine));
  }
}

//===========================================
// FShuffledCalc::onButtonClick
//===========================================
void FShuffledCalc::onButtonClick(int id) {
  if (id <= 9) {
    m_calculator.number(id);
  }
  else {
    switch (id) {
      case BTN_PLUS:
        m_calculator.plus();
        break;
      case BTN_MINUS:
        m_calculator.minus();
        break;
      case BTN_TIMES:
        m_calculator.times();
        break;
      case BTN_DIVIDE:
        m_calculator.divide();
        break;
      case BTN_POINT:
        m_calculator.point();
        break;
      case BTN_EQUALS: {
        m_calculator.equals();
        break;
      }
      case BTN_CLEAR:
        m_calculator.clear();
        break;
    }
  }

  QString symbols = translateToSymbols(m_calculator.display().c_str());
  m_wgtDigitDisplay->setText(symbols);

  if (m_calculator.display() == m_targetValue) {
    auto& buttons = m_wgtButtonGrid->buttons;
    int i = 0;
    int n = m_updateLoop->fps() / 2;

    m_updateLoop->add([&, i, n]() mutable {
      for (auto it = buttons.begin(); it != buttons.end(); ++it) {
        QSize sz = (*it)->size();
        (*it)->resize(sz * 0.8);

        if (i == n - 1) {
          (*it)->setVisible(false);
        }
      }

      ++i;
      return i < n;
    }, [&]() {
      m_wgtDigitDisplay->setText("");
      int i = 0;
      int n = m_updateLoop->fps() / 2;

      m_updateLoop->add([&, i, n]() mutable {
        int x = m_wgtDigitDisplay->geometry().x();
        int y = m_wgtDigitDisplay->geometry().y();
        m_wgtDigitDisplay->move(x, y + 30);

        ++i;
        return i < n;
      },
      [&]() {
        parentFragData<FMainData>().eventSystem.fire(RequestStateChangeEvent(ST_LOGIN_SCREEN));
      });
    });
  }
}
