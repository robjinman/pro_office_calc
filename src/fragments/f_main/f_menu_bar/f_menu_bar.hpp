#ifndef __PROCALC_FRAGMENTS_F_MENU_BAR_HPP__
#define __PROCALC_FRAGMENTS_F_MENU_BAR_HPP__


#include <memory>
#include <QObject>
#include <QAction>
#include <QString>
#include <QMenu>
#include "fragment.hpp"


struct FMenuBarData : public FragmentData {
  std::unique_ptr<QMenu> mnuFile;
  std::unique_ptr<QMenu> mnuHelp;
  std::unique_ptr<QAction> actAbout;
  std::unique_ptr<QAction> actQuit;
};

class FMenuBar : public QObject, public Fragment {
  Q_OBJECT

  public:
    FMenuBar(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  private slots:
    void showAbout();
    void onClose();

  private:
    FMenuBarData m_data;

    QString m_aboutDialogTitle;
    QString m_aboutDialogText;
};


#endif
