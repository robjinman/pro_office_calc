#ifndef __PROCALC_FRAGMENTS_F_APP_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_APP_DIALOG_HPP__


#include <memory>
#include <QDialog>
#include "fragment.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"
#include "event_system.hpp"


class DialogClosedEvent : public Event {
  public:
    DialogClosedEvent(const std::string& name)
      : Event("dialogClosed"),
        name(name) {}

    std::string name;
};

class FAppDialogData : public WidgetFragData {
  public:
    FAppDialogData()
      : WidgetFragData(makeQtObjPtr<QVBoxLayout>()) {}
};

class FAppDialog : public QDialog, public Fragment {
  Q_OBJECT

  public:
    FAppDialog(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FAppDialog() override;

  protected:
     void keyPressEvent(QKeyEvent* e) override;
     void closeEvent(QCloseEvent* e) override;

  private:
    FAppDialogData m_data;

    std::string m_name;
    EventHandle m_hShow;
};


#endif
