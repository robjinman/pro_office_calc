#ifndef __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_TROUBLESHOOTER_DIALOG_HPP__


#include <memory>
#include <QDialog>
#include <QAction>
#include <QVBoxLayout>
#include "fragment.hpp"


class EventSystem;
class UpdateLoop;

struct FTroubleshooterDialogData : public FragmentData {
  EventSystem* eventSystem;
  UpdateLoop* updateLoop;
  std::unique_ptr<QAction> actPreferences;
  std::unique_ptr<QVBoxLayout> vbox;
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
};


#endif
