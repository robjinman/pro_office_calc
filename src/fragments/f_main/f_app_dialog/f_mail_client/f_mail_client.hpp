#ifndef __PROCALC_FRAGMENTS_F_MAIL_CLIENT_HPP__
#define __PROCALC_FRAGMENTS_F_MAIL_CLIENT_HPP__


#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextBrowser>
#include <QLabel>
#include <QMargins>
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"


struct FMailClientData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<QTabWidget> wgtTabs;

  struct {
    QtObjPtr<QWidget> page;
    QtObjPtr<QVBoxLayout> vbox;
    QtObjPtr<QTableWidget> wgtTable;
  } inboxTab;

  struct {
    QtObjPtr<QWidget> page;
    QtObjPtr<QGridLayout> grid;
    QtObjPtr<QTextBrowser> wgtText;
    QtObjPtr<QLabel> wgtFrom;
    QtObjPtr<QLabel> wgtSubject;
    QtObjPtr<QLabel> wgtTo;
    QtObjPtr<QLabel> wgtAttachments;
  } emailTab;
};

class FMailClient : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FMailClient(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMailClient() override;

  private slots:
    void onCellDoubleClick(int row, int col);
    void onTabClose(int idx);

  private:
    FMailClientData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;

    bool m_serverRoomLaunched = false;
    int m_larryKilledId = -1;

    const int ST_INITIAL = 0;
    const int ST_LARRY_DEAD = 1;

    int m_inboxState = 0;

    void setupInboxTab();
    void setupEmailTab();
    void populateInbox();
    void enableEmails(int startIdx, int num);
};


#endif
