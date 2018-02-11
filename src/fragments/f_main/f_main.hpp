#ifndef __PROCALC_FRAGMENTS_F_MAIN_HPP__
#define __PROCALC_FRAGMENTS_F_MAIN_HPP__


#include <memory>
#include <functional>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include "update_loop.hpp"
#include "fragment.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"


struct FMainData : public WidgetFragData {
  FMainData()
    : WidgetFragData(new QVBoxLayout) {}

  QWidget* wgtCentral;
  std::unique_ptr<QMenu> mnuFile;
  std::unique_ptr<QAction> actQuit;
  std::unique_ptr<QMenu> mnuHelp;
  std::unique_ptr<QAction> actAbout;
  std::function<void()> fnOnQuit = []() {};
};


class FMain : public QMainWindow, public Fragment {
  Q_OBJECT

  public:
    FMain(const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMain();

  protected:
    virtual void closeEvent(QCloseEvent*);

  private slots:
    void showAbout();

  private:
    FMainData m_data;
    QString m_aboutDialogTitle;
    QString m_aboutDialogText;
};


#endif
