#ifndef __PROCALC_FRAGMENTS_F_ABOUT_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_ABOUT_DIALOG_HPP__


#include <memory>
#include <QObject>
#include <QAction>
#include <QString>
#include <QMenu>
#include "fragment.hpp"


struct FAboutDialogData : public FragmentData {
  std::unique_ptr<QMenu> mnuHelp;
  std::unique_ptr<QAction> actAbout;
};

class FAboutDialog : public QObject, public Fragment {
  Q_OBJECT

  public:
    FAboutDialog(Fragment& parent, FragmentData& parentData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FAboutDialog() override;

  private slots:
    void showAbout();

  private:
    FAboutDialogData m_data;

    QString m_aboutDialogTitle;
    QString m_aboutDialogText;
};


#endif
