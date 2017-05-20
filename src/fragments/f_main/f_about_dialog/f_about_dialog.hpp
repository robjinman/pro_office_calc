#ifndef __PROCALC_FRAGMENTS_F_ABOUT_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_ABOUT_DIALOG_HPP__


#include <memory>
#include <QObject>
#include <QAction>
#include <QString>
#include "fragment.hpp"


struct FAboutDialogData : public FragmentData {};

class FAboutDialog : public QObject, public Fragment {
  Q_OBJECT

  public:
    FAboutDialog(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  private slots:
    void showAbout();

  private:
    FAboutDialogData m_data;
    std::unique_ptr<QAction> m_actAbout;
    QString m_text;
};


#endif
