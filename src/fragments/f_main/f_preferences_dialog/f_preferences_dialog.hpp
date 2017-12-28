#ifndef __PROCALC_FRAGMENTS_F_PREFERENCES_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_PREFERENCES_DIALOG_HPP__


#include <memory>
#include <QDialog>
#include <QAction>
#include <QVBoxLayout>
#include "fragment.hpp"


class EventSystem;
class UpdateLoop;

struct FPreferencesDialogData : public FragmentData {
  EventSystem* eventSystem;
  UpdateLoop* updateLoop;
  std::unique_ptr<QAction> actPreferences;
  std::unique_ptr<QVBoxLayout> vbox;
};

class FPreferencesDialog : public QDialog, public Fragment {
  Q_OBJECT

  public:
    FPreferencesDialog(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FPreferencesDialog() override;

  private slots:
    void showPreferencesDialog();

  private:
    FPreferencesDialogData m_data;
};


#endif
