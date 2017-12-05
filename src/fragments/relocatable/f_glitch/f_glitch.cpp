#include "fragments/relocatable/f_glitch/f_glitch.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"
#include "effects.hpp"


//===========================================
// FGlitch::FGlitch
//===========================================
FGlitch::FGlitch(Fragment& parent_, FragmentData& parentData_)
  : QLabel(nullptr),
    Fragment("FGlitch", parent_, parentData_, m_data) {}

//===========================================
// FGlitch::rebuild
//===========================================
void FGlitch::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<QWidget>();

  setParent(&parent);

  setGeometry(parent.rect());
  setScaledContents(true);

  m_glitchBuffer.reset(new QImage(parent.size(), QImage::Format_ARGB32));
  m_glitchTimer.reset(new QTimer(this));

  connect(m_glitchTimer.get(), SIGNAL(timeout()), this, SLOT(tick()));

  m_glitchTimer->start(1000);
  show();

  Fragment::rebuild(spec_);
}

//===========================================
// FGlitch::tick
//===========================================
void FGlitch::tick() {
  auto& parent = parentFrag<QWidget>();

  if (!isVisible()) {
    QImage buf(m_glitchBuffer->size(), m_glitchBuffer->format());

    parent.render(&buf);
    garbleImage(buf, *m_glitchBuffer);

    setPixmap(QPixmap::fromImage(*m_glitchBuffer));
    setVisible(true);
    raise();

    m_glitchTimer->setInterval(100);
  }
  else {
    setVisible(false);

    std::uniform_int_distribution<int> dist(100, 2000);
    m_glitchTimer->setInterval(dist(m_randEngine));
  }
}

//===========================================
// FGlitch::cleanUp
//===========================================
void FGlitch::cleanUp() {
  setParent(nullptr);
}
