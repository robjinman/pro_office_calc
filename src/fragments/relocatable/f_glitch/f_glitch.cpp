#include "fragments/relocatable/f_glitch/f_glitch.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"
#include "effects.hpp"
#include "utils.hpp"


//===========================================
// FGlitch::FGlitch
//===========================================
FGlitch::FGlitch(Fragment& parent_, FragmentData& parentData_)
  : QLabel(nullptr),
    Fragment("FGlitch", parent_, parentData_, m_data) {

  DBG_PRINT("FGlitch::FGlitch\n");
}

//===========================================
// FGlitch::initialise
//===========================================
void FGlitch::initialise(const FragmentSpec& spec_) {
  DBG_PRINT("FGlitch::initialise\n");

  auto& parent = parentFrag<QWidget>();

  setParent(&parent);

  setScaledContents(true);
  setAttribute(Qt::WA_TransparentForMouseEvents);

  m_glitchTimer.reset(new QTimer(this));

  connect(m_glitchTimer.get(), SIGNAL(timeout()), this, SLOT(tick()));

  show();
}

//===========================================
// FGlitch::reload
//===========================================
void FGlitch::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FGlitch::reload\n");

  auto& spec = dynamic_cast<const FGlitchSpec&>(spec_);
  auto& parent = parentFrag<QWidget>();

  move(0, 0);
  resize(parent.size());

  m_glitchBuffer.reset(new QImage(parent.size(), QImage::Format_ARGB32));

  m_glitchFreqMin = spec.glitchFreqMin;
  m_glitchFreqMax = spec.glitchFreqMax;
  m_glitchDuration = spec.glitchDuration;

  m_glitchTimer->start(m_glitchFreqMin * 1000);
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

    m_glitchTimer->setInterval(m_glitchDuration * 1000);
  }
  else {
    setVisible(false);

    std::uniform_int_distribution<int> dist(m_glitchFreqMin * 1000, m_glitchFreqMax * 1000);
    m_glitchTimer->setInterval(dist(m_randEngine));
  }
}

//===========================================
// FGlitch::cleanUp
//===========================================
void FGlitch::cleanUp() {
  DBG_PRINT("FGlitch::cleanUp\n");

  setParent(nullptr);
}

//===========================================
// FGlitch::~FGlitch
//===========================================
FGlitch::~FGlitch() {
  DBG_PRINT("FGlitch::~FGlitch\n");
}
