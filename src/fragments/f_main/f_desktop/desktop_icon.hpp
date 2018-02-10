#ifndef __PROCALC_FRAGMENTS_F_DESKTOP_DESKTOP_ICON_HPP__
#define __PROCALC_FRAGMENTS_F_DESKTOP_DESKTOP_ICON_HPP__


#include <memory>
#include <string>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>


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
    std::unique_ptr<QVBoxLayout> m_vbox;
    std::unique_ptr<QPushButton> m_wgtButton;
    std::unique_ptr<QLabel> m_wgtText;

    long long m_lastClick = 0;

};


#endif
