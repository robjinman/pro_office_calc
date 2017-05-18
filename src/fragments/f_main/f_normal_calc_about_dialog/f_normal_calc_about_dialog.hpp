#ifndef __PROCALC_FRAGMENTS_F_NORMAL_CALC_ABOUT_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_NORMAL_CALC_ABOUT_DIALOG_HPP__


#include <memory>
#include <QObject>
#include <QAction>
#include "fragment.hpp"


class QAction;

struct FNormalCalcAboutDialogData : public FragmentData {};

class FNormalCalcAboutDialog : public QObject, public Fragment {
  Q_OBJECT

  public:
    FNormalCalcAboutDialog(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  private slots:
    void showAbout();

  private:
    void onQuit();

    FNormalCalcAboutDialogData m_data;
    std::unique_ptr<QAction> m_actAbout;
    int m_stateId;
};


#endif
