#ifndef __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_HPP__


#include <QDialog>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>
#include <QTextBrowser>
#include <QTabWidget>
#include <QTimer>
#include "raycast/raycast_widget.hpp"
#include "fragments/f_main/f_troubleshooter_dialog/game_logic.hpp"
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"


struct FTroubleshooterDialogData : public FragmentData {
  QtObjPtr<QAction> actPreferences;
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<QTabWidget> wgtTabs;

  struct {
    QtObjPtr<QWidget> page;
    QtObjPtr<QVBoxLayout> vbox;
    QtObjPtr<QLabel> wgtCaption;
    QtObjPtr<QPushButton> wgtRunTroubleshooter;
    QtObjPtr<QProgressBar> wgtProgressBar;
    QtObjPtr<QGroupBox> wgtGroupbox;
    QtObjPtr<QVBoxLayout> resultsVbox;
    QtObjPtr<QHBoxLayout> btnsHbox;
    QtObjPtr<QLabel> wgtNoProblemsFound;
    QtObjPtr<QLabel> wgtProblemResolved;
    QtObjPtr<QPushButton> wgtYes;
    QtObjPtr<QPushButton> wgtNo;
    QtObjPtr<QTimer> timer;
  } tab1;

  struct {
    QtObjPtr<QWidget> page;
    QtObjPtr<QVBoxLayout> vbox;
    QtObjPtr<QTextBrowser> wgtTextBrowser;
  } tab2;

  struct {
    QtObjPtr<QWidget> page;
    QtObjPtr<QVBoxLayout> vbox;
    QtObjPtr<RaycastWidget> wgtRaycast;
    std::unique_ptr<its_raining_tetrominos::GameLogic> gameLogic;
  } tab3;
};

class FTroubleshooterDialog : public QDialog, public Fragment {
  Q_OBJECT

  public:
    FTroubleshooterDialog(Fragment& parent, FragmentData& parentData,
      const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FTroubleshooterDialog() override;

  private slots:
    void showTroubleshooterDialog();
    void onRunTroubleshooter();
    void onTick();
    void onNoClick();
    void onYesClick();

  private:
    FTroubleshooterDialogData m_data;

    void setupTab1(int raycastWidth, int raycastHeight);
    void setupTab2();
    void setupTab3(int raycastWidth, int raycastHeight);
};


#endif
