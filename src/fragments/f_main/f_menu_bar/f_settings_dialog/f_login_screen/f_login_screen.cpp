#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_login_screen/f_login_screen.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_login_screen/f_login_screen_spec.hpp"
#include "effects.hpp"


//===========================================
// FLoginScreen::FLoginScreen
//===========================================
FLoginScreen::FLoginScreen(Fragment& parent_, FragmentData& parentData_)
  : QLabel(nullptr),
    Fragment("FLoginScreen", parent_, parentData_, m_data) {}

//===========================================
// FLoginScreen::rebuild
//===========================================
void FLoginScreen::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<FSettingsDialog>();
  auto& parentData = parentFragData<FSettingsDialogData>();

  m_origParentState.spacing = parentData.vbox->spacing();
  m_origParentState.margins = parentData.vbox->contentsMargins();

  parentData.vbox->setSpacing(0);
  parentData.vbox->setContentsMargins(0, 0, 0, 0);
  parentData.vbox->addWidget(this);

  m_glitchOverlay.reset(new QLabel(this));
  m_glitchBuffer.reset(new QImage(size(), QImage::Format_ARGB32));
  m_glitchTimer.reset(new QTimer(this));
  m_glitchOverlay->setGeometry(rect());
  m_glitchOverlay->setScaledContents(true);
  m_glitchOverlay->show();

  connect(m_glitchTimer.get(), SIGNAL(timeout()), this, SLOT(tick()));

  m_glitchTimer->start(1000);

  auto& spec = dynamic_cast<const FLoginScreenSpec&>(spec_);

  QPixmap tmp(spec.backgroundImage);
  m_data.background.reset(new QPixmap(tmp.scaledToHeight(parent.size().height())));

  setPixmap(*m_data.background);

  Fragment::rebuild(spec_);
}

//===========================================
// FLoginScreen::tick
//===========================================
void FLoginScreen::tick() {
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
// FLoginScreen::cleanUp
//===========================================
void FLoginScreen::cleanUp() {
  auto& parentData = parentFragData<FSettingsDialogData>();

  parentData.vbox->setSpacing(m_origParentState.spacing);
  parentData.vbox->setContentsMargins(m_origParentState.margins);
  parentData.vbox->removeWidget(this);
}
