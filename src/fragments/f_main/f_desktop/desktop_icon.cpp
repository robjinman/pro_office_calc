#include <QDateTime>
#include <QPixmap>
#include "fragments/f_main/f_desktop/desktop_icon.hpp"


using std::string;
using std::unique_ptr;


const long long DOUBLE_CLICK_DELAY = 200;


//===========================================
// DesktopIcon::DesktopIcon
//===========================================
DesktopIcon::DesktopIcon(const std::string& name, const string& image, const string& text)
  : QWidget(nullptr),
    m_name(name) {

  QPixmap* pix = new QPixmap(image.c_str());

  m_wgtButton = makeQtObjPtr<QPushButton>(*pix, "");
  m_wgtText = makeQtObjPtr<QLabel>(text.c_str());
  m_vbox = makeQtObjPtr<QVBoxLayout>();

  setLayout(m_vbox.get());

  m_wgtButton->setFlat(true);
  m_wgtButton->setIconSize(pix->rect().size());
  m_wgtButton->setMinimumWidth(pix->width());
  m_wgtButton->setMinimumHeight(pix->height());
  m_wgtButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  m_vbox->setSpacing(0);
  m_vbox->setMargin(0);

  m_vbox->addWidget(m_wgtButton.get());
  m_vbox->addWidget(m_wgtText.get());
  m_vbox->addStretch(1);

  connect(m_wgtButton.get(), SIGNAL(clicked()), this, SLOT(onButtonClick()));
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
