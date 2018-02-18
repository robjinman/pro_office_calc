#ifndef __PROCALC_FRAGMENTS_F_MAIN_HPP__
#define __PROCALC_FRAGMENTS_F_MAIN_HPP__


#include <functional>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include "update_loop.hpp"
#include "fragment.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"
#include "qt_obj_ptr.hpp"


struct FMainData : public WidgetFragData {
  FMainData()
    : WidgetFragData(new QVBoxLayout) {}

  QWidget* wgtCentral;
  QtObjPtr<QMenu> mnuFile;
  QtObjPtr<QAction> actQuit;
  QtObjPtr<QMenu> mnuHelp;
  QtObjPtr<QAction> actAbout;
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
