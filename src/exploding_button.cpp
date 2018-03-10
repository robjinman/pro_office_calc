#include <cmath>
#include <QPixmap>
#include <QPainter>
#include "exploding_button.hpp"
#include "utils.hpp"
#include "update_loop.hpp"


//===========================================
// ExplodingButton::ExplodingButton
//===========================================
ExplodingButton::ExplodingButton(QWidget* parent, const QString& caption, UpdateLoop& updateLoop)
  : QPushButton(caption, parent),
    m_updateLoop(updateLoop) {

  m_spriteSheet.reset(new QImage("data/explosion.png"));

  m_wgtLabel = makeQtObjPtr<QLabel>(parent);
  m_wgtLabel->setFixedSize(100, 100);
  m_wgtLabel->hide();

  connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

//===========================================
// ExplodingButton::onClick
//===========================================
void ExplodingButton::onClick() {
  DBG_PRINT("Explode!\n");

  QPoint A = geometry().center();
  QPoint B = m_wgtLabel->geometry().center();

  m_wgtLabel->move(m_wgtLabel->pos() + (A - B));
  m_wgtLabel->show();

  int nFrames = 5;
  int ticks = 10;
  int frameW = m_spriteSheet->width() / nFrames;
  int frameH = m_spriteSheet->height();
  double ticksPerFrame = static_cast<double>(ticks) / nFrames;

  QPixmap pixmap(100, 100);

  hide();

  int f = 0;
  m_updateLoop.add([=]() mutable {
    pixmap.fill(QColor(0, 0, 0, 0));

    QPainter painter;
    painter.begin(&pixmap);

    int i = static_cast<int>(static_cast<double>(f) / ticksPerFrame);
    int frameX = i * frameW;

    QRect trg(QPoint(0, 0), m_wgtLabel->size());
    QRect src(frameX, 0, frameW, frameH);

    painter.drawImage(trg, *m_spriteSheet, src);

    m_wgtLabel->setPixmap(pixmap);

    return ++f < ticks;
  }, [this]() {
    m_wgtLabel->hide();
  });
}

//===========================================
// ExplodingButton::~ExplodingButton
//===========================================
ExplodingButton::~ExplodingButton() {}
