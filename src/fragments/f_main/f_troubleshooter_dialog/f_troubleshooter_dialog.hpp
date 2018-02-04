#ifndef __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_HPP__


#include <memory>
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
#include "fragment.hpp"


class EventSystem;
class UpdateLoop;

struct FTroubleshooterDialogData : public FragmentData {
  EventSystem* eventSystem;
  UpdateLoop* updateLoop;

  std::unique_ptr<QAction> actPreferences;
  std::unique_ptr<QVBoxLayout> vbox;
  std::unique_ptr<QTabWidget> wgtTabs;

  struct {
    std::unique_ptr<QWidget> page;
    std::unique_ptr<QVBoxLayout> vbox;
    std::unique_ptr<QLabel> wgtCaption;
    std::unique_ptr<QPushButton> wgtRunTroubleshooter;
    std::unique_ptr<QProgressBar> wgtProgressBar;
    std::unique_ptr<QGroupBox> wgtGroupbox;
    std::unique_ptr<QVBoxLayout> resultsVbox;
    std::unique_ptr<QHBoxLayout> btnsHbox;
    std::unique_ptr<QLabel> wgtNoProblemsFound;
    std::unique_ptr<QLabel> wgtProblemResolved;
    std::unique_ptr<QPushButton> wgtYes;
    std::unique_ptr<QPushButton> wgtNo;
  } tab1;

  struct {
    std::unique_ptr<QWidget> page;
    std::unique_ptr<QVBoxLayout> vbox;
    std::unique_ptr<QTextBrowser> wgtTextBrowser;
  } tab2;

  struct {
    std::unique_ptr<QWidget> page;
    std::unique_ptr<QVBoxLayout> vbox;
  } tab3;
};

class FTroubleshooterDialog : public QDialog, public Fragment {
  Q_OBJECT

  public:
    FTroubleshooterDialog(Fragment& parent, FragmentData& parentData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FTroubleshooterDialog() override;

  private slots:
    void showTroubleshooterDialog();

  private:
    FTroubleshooterDialogData m_data;

    void setupTab1();
    void setupTab2();
    void setupTab3();
};


#endif
