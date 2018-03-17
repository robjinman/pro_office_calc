#include <QDateTime>
#include <QPixmap>
#include "fragments/f_main/f_desktop/desktop_icon.hpp"


using std::string;
using std::unique_ptr;


const long long DOUBLE_CLICK_DELAY = 300;


//===========================================
// DesktopIcon::DesktopIcon
//===========================================
DesktopIcon::DesktopIcon(const std::string& name, const string& image, const string& text)
  : QWidget(nullptr),
    m_name(name) {

  QPixmap pix(image.c_str());

  wgtButton = makeQtObjPtr<QPushButton>(pix, "");
  wgtText = makeQtObjPtr<QLabel>(text.c_str());
  m_vbox = makeQtObjPtr<QVBoxLayout>();

  setLayout(m_vbox.get());

  wgtButton->setFlat(true);
  wgtButton->setIconSize(pix.rect().size());
  wgtButton->setMinimumWidth(pix.width());
  wgtButton->setMinimumHeight(pix.height());
  wgtButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  m_vbox->setSpacing(0);
  m_vbox->setMargin(0);

  m_vbox->addWidget(wgtButton.get());
  m_vbox->addWidget(wgtText.get());
  m_vbox->addStretch(1);

  connect(wgtButton.get(), SIGNAL(clicked()), this, SLOT(onButtonClick()));
}

//===========================================
// DesktopIcon::onButtonClick
//===========================================
void DesktopIcon::onButtonClick() {
  long long now = QDateTime::currentMSecsSinceEpoch();
  if (now - m_lastClick <= DOUBLE_CLICK_DELAY) {
    emit activated(m_name);
  }

  m_lastClick = now;
}

//===========================================
// DesktopIcon::~DesktopIcon
//===========================================
DesktopIcon::~DesktopIcon() {}
