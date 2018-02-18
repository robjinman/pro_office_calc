#ifndef __PROCALC_FRAGMENTS_F_DESKTOP_DESKTOP_ICON_HPP__
#define __PROCALC_FRAGMENTS_F_DESKTOP_DESKTOP_ICON_HPP__


#include <string>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "qt_obj_ptr.hpp"


class DesktopIcon : public QWidget {
  Q_OBJECT

  public:
    DesktopIcon(const std::string& name, const std::string& image, const std::string& text);

    virtual ~DesktopIcon() override;

  signals:
    void activated(const std::string& name);

  private slots:
    void onButtonClick();

  private:
    std::string m_name;
    QtObjPtr<QVBoxLayout> m_vbox;
    QtObjPtr<QPushButton> m_wgtButton;
    QtObjPtr<QLabel> m_wgtText;

    long long m_lastClick = 0;

};


#endif
