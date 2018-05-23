#include <vector>
#include <array>
#include <QPushButton>
#include <QHeaderView>
#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_mail_client/f_mail_client.hpp"
#include "fragments/f_main/f_app_dialog/f_mail_client/f_mail_client_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"
#include "app_config.hpp"


using std::vector;
using std::to_string;


namespace youve_got_mail {
  extern const std::string exitDoorCode;
}


struct Email {
  QString subject;
  QString from;
  QString to;
  QString date;

  QString subjectGarbled;
  QString fromGarbled;
  QString dateGarbled;

  QString body;

  QString attachment;
};

// Emails in chronological order
const std::array<Email, 11> EMAILS = {{
  // 0
  {
    "Meeting to discuss y2k issue",
    "Brian Williams",
    "devteam",
    "11/03/95 12:55:36",

    "M̨e̴eti͟ng ҉to d͜i̵sc̛u̕ss̀ y2ḱ i̛ssúe̸",
    "B̡r̸i̶a͘n ̨W̢iļl̸i̕a͠m҉s",
    "11͟/̧03̧/̛95̷ ͠1̢2:͝5̷5̀:͏3͏6҉",

    // Message body
    //
    "I've arranged a meeting for tomorrow afternoon to discuss the y2k or \"millenium bug\" issue. "
    "See the attached document for a brief overview.\n\n"

    "Brian Williams,\n"
    "Lead Kernel Developer,\n"
    "Apex Systems\n",

    "y2k_threat.doc"
  },
  // 1
  {
    "Potentially dangerous app - remove ASAP",
    "Alan Shand",
    "devteam",
    "12/03/95 13:06:44",

    "Pǫt̡entia҉l̵ly̕ ̢dangero͢u̵s̴ ͏app̡ ̕-͝ re̛m̴o͜vę A̕SAP",
    "Al̛a̕n S̵han̡ḑ",
    "1̵2/͞0͜3̸/̛95͡ ́13:͘0͠6:4͜4",

    // Message body
    //
    "Just a quick update before I leave.\n\n"

    "This is an urgent request to retract validation of Pro Office Calculator due to potentially "
    "dangerous behaviour not uncovered during initial testing. I've had Brian and his team look "
    "into this. It's clearly doing something odd, but they're unsure what.\n\n"

    "In my absence, please ensure this application does NOT ship with the upcoming release of Apex "
    "OS.\n\n"

    "See you all next week.\n\n"

    "Alan Shand,\n"
    "Chief QA Officer,\n"
    "Apex Systems\n"
  },
  // 2
  {
    "Quick question",
    "Alan Shand",
    "devteam",
    "12/03/95 13:08:17",

    "Q͏uic͟k̢ q̷u͝e̛sţio͝ņ",
    "Àl͞a̴n̨ Sha̕nd",
    "12/̀03͞/͞9̀5̨ ͘1͟3:͢08:1̀7",

    // Message body
    //
    "Sorry, one more thing. Is Rob Jinman one of our ex-contractors?\n\n"

    "Alan Shand,\n"
    "Chief QA Officer,\n"
    "Apex Systems\n"
  },
  // Outage happens here when you destroy the servers, which erases the last three messages.
  // Mysteriously, an email from Rob Jinman seems to have appeared while the servers were down.
  //
  // 3
  {
    "Please Read",
    "Rob Jinman",
    "larry",
    "12/03/95 13:22:20",

    "Please Read",
    "Rob Jinman",
    "12/03/95 13:22:20",

    // Message body
    //
    QString("Good work. The code for the exit door is ") + youve_got_mail::exitDoorCode.c_str() +
      ".\n\n"
    "Rob\n"
  },
  // Servers come back online
  //
  // 4
  {
    "Mail server outage",
    "Richard Nelms",
    "all",
    "12/03/95 14:01:23",

    "Mail server outage",
    "Richard Nelms",
    "12/03/95 14:01:23",

    // Message body
    //
    "We had an outage of the mail servers this afternoon between around 13:10 and 13:40. It's "
    "likely that some mail was lost.\n\n"

    "Richard Nelms,\n"
    "Systems Administator,\n"
    "Apex Systems\n"
  },
  // 5
  {
    "Larry?",
    "Daniel Bright",
    "Me",
    "12/03/95 13:09:25",

    "L͝a͡rry̢?̛",
    "Dani̴el̨ B̀r̴ig̸h̸t̡",
    "12/03́/͢9͡5҉ 13:̀09:̕25̶",

    // Message body
    //
    "You weren't in the meeting earlier. Did you forget? Dave was trying to get hold of you this "
    "afternoon also. In future could you let someone know before disappearing like that?\n\n"

    "Daniel Bright,\n"
    "Application Development Manager,\n"
    "Apex Systems\n"
  },
  // 6
  {
    "Re: Quick question",
    "Paul Gresham",
    "devteam",
    "12/03/95 14:04:35",

    "Ŗe: ̨Q͟uick ̴q̛u̸esti͝ón̨",
    "Paul͢ ̨Gr͘es̵ham̷",
    "12/̴03̧/95͢ 1̢4:0̕4̵:͝3̢5",

    // Message body
    //
    "Hi Alan,\n\n"

    "Yes, he one day dropped off the radar and we haven't seen him since - never even got his last "
    "invoice IIRC.\n\n"

    "Paul Gresham,\n"
    "Systems Architect,\n"
    "Apex Systems\n"
  },
  // 7
  {
    "Latest regarding Pro Office Calc",
    "Brian Williams",
    "devteam",
    "12/03/95 16:20:09",

    "La̧tes҉t r͟eg̴ard͢iǹg Pr̡o ͏Of̕fíc͏e ͞Cal̛ç",
    "B̀ri̧a͘n̢ ̷W̷i̢l̕l̨i͞a̕ms̢",
    "12/̧0҉3͢/̡9̶5 ̶1́6:̧2̨0͟:0̧9",

    // Message body
    //
    "It's using library functions and system calls that weren't implemented at the time the app "
    "was submitted. That doesn't make any sense.\n\n"

    "Brian Williams,\n"
    "Lead Kernel Developer,\n"
    "Apex Systems\n",

    "syscalls.txt"
  },
  // 8
  {
    "re: Latest regarding Pro Office Calc",
    "Michael Considine",
    "devteam",
    "12/03/95 16:38:12",

    "r͟e͏: ҉Latest r͏e͞ga͢r͞dińg ͝P̛ro͠ O͡f̴fic͏e̴ ͝C͟a͝l͠c̀",
    "M̀i̧çha͜e͟l͡ ̵Con͢s͡id͜i҉n͡e",
    "12/͡0͏3̧/95͘ ͠16:38:̀12",

    // Message body
    //
    "We hadn't even thought of those functions yet. How is that possible?\n\n"

    "Michael Considine,\n"
    "Senior Kernel Developer,\n"
    "Apex Systems\n"
  },
  // 9
  {
    "re: re: Latest regarding Pro Office Calc",
    "Brian Williams",
    "devteam",
    "12/03/95 16:42:19",

    "re: re:͘ ̡Ļate̡s̢t ̴r̴e̶gar̴d͝ing̛ ͞P͠r͏ó O̴f҉f͜ice̴ Ca̵lc",
    "Br̨i̵an W̛i̴l͜l̴i̕am̷s͢",
    "1̴2/0̀3/͡95̀ ́16:42:19",

    // Message body
    //
    "It's not.\n\n"

    "Brian Williams,\n"
    "Lead Kernel Developer,\n"
    "Apex Systems\n"
  },
  // 10
  {
    "y2k issue",
    "Brian Williams",
    "devteam",
    "12/03/95 17:10:58",

    "y2k issue",
    "Brian Williams",
    "12/03/95 17:10:58",

    // Message body
    //
    "Following our meeting earlier, it's become clear that the Y2K problem should take priority "
    "over the coming weeks. Expect changes in your schedule/workload to reflect this.\n\n"

    "Brian Williams,\n"
    "Lead Kernel Developer,\n"
    "Apex Systems\n"
  },
}};

const vector<vector<int>> INBOX_STATES = {
  { 0, 1, 2, 6, 7, 8, 9 },    // ST_INITIAL
  { 0, 1, 2, 5, 6, 7, 8, 9 }, // ST_LARRY_DEAD
  { 0, 3, 4, 5, 10 }          // ST_SERVERS_DESTROYED
};

//===========================================
// getEmails
//===========================================
static vector<const Email*> getEmails(int state) {
  vector<const Email*> emails;
  for (int idx : INBOX_STATES[state]) {
    emails.push_back(&EMAILS[idx]);
  }
  return emails;
}

//===========================================
// FMailClient::FMailClient
//===========================================
FMailClient::FMailClient(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FMailClient", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FMailClient::FMailClient\n");
}

//===========================================
// FMailClient::reload
//===========================================
void FMailClient::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FMailClient::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  m_data.vbox = makeQtObjPtr<QVBoxLayout>();
  setLayout(m_data.vbox.get());

  m_data.wgtTabs = makeQtObjPtr<QTabWidget>(this);
  m_data.wgtTabs->setTabsClosable(true);
  m_data.vbox->addWidget(m_data.wgtTabs.get());

  setupInboxTab();
  setupEmailTab();

  connect(m_data.wgtTabs.get(), SIGNAL(tabCloseRequested(int)), this, SLOT(onTabClose(int)));

  commonData.eventSystem.listen("youveGotMail/larryKilled", [this](const Event&) {
    m_inboxState = ST_LARRY_DEAD;
    populateInbox();
    enableEmails(0, -1);
  }, m_larryKilledId);

  commonData.eventSystem.listen("youveGotMail/serversDestroyed", [this](const Event&) {
    m_inboxState = ST_SERVERS_DESTROYED;
    m_data.wgtTabs->removeTab(1);
    populateInbox();
    enableEmails(0, -1);
  }, m_serversDestroyedId);
}

//===========================================
// FMailClient::onTabClose
//===========================================
void FMailClient::onTabClose(int idx) {
  if (idx > 0) {
    m_data.wgtTabs->removeTab(idx);
  }
}

//===========================================
// constructTableItem
//===========================================
static QTableWidgetItem* constructTableItem(const QString& text) {
  static const Qt::ItemFlags disableFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable |
    Qt::ItemIsSelectable | Qt::ItemIsEditable;

  QTableWidgetItem* item = new QTableWidgetItem(text);
  item->setFlags(item->flags() & ~disableFlags);

  return item;
}

//===========================================
// constructInboxEntry
//===========================================
static void constructInboxEntry(QTableWidget& table, int row, const QString& subject,
  bool attachment, const QString& from, const QString& date) {

  QTableWidgetItem* subItem = constructTableItem(subject);
  QTableWidgetItem* attItem = constructTableItem(attachment ? "✓" : "");
  QTableWidgetItem* fromItem = constructTableItem(from);
  QTableWidgetItem* dateItem = constructTableItem(date);

  table.setItem(row, 0, subItem);
  table.setItem(row, 1, attItem);
  table.setItem(row, 2, fromItem);
  table.setItem(row, 3, dateItem);
}

//===========================================
// enableRow
//===========================================
static void enableRow(QTableWidget& table, int row) {
  for (int i = 0; i < table.columnCount(); ++i) {
    QTableWidgetItem* item = table.item(row, i);

    if (item != nullptr) {
      item->setFlags(item->flags() | Qt::ItemIsEnabled);
    }
  }
}

//===========================================
// FMailClient::enableEmails
//
// If num is -1, all emails will be enabled
//===========================================
void FMailClient::enableEmails(int startIdx, int num) {
  vector<const Email*> emails = getEmails(m_inboxState);
  auto& inbox = m_data.inboxTab;

  if (num == -1) {
    num = emails.size();
  }

  for (int idx = startIdx; idx < startIdx + num; ++idx) {
    if (idx >= static_cast<int>(emails.size())) {
      break;
    }

    auto& email = *emails[idx];

    enableRow(*inbox.wgtTable, idx);
    inbox.wgtTable->item(idx, 0)->setText(email.subject);
    if (email.attachment != "") {
      inbox.wgtTable->item(idx, 1)->setText("✓");
    }
    inbox.wgtTable->item(idx, 2)->setText(email.from);
    inbox.wgtTable->item(idx, 3)->setText(email.date);
  }
}

//===========================================
// FMailClient::onCellDoubleClick
//===========================================
void FMailClient::onCellDoubleClick(int row, int col) {
  vector<const Email*> emails = getEmails(m_inboxState);

  auto& email = *emails[row];
  auto& tab = m_data.emailTab;
  auto& inbox = m_data.inboxTab;

  if (inbox.wgtTable->item(row, col)->flags() & Qt::ItemIsEnabled) {
    m_data.wgtTabs->setCurrentIndex(1);

    tab.wgtText->setText(email.body);
    tab.wgtFrom->setText(email.from);
    tab.wgtDate->setText(email.date);
    tab.wgtSubject->setText(email.subject);
    tab.wgtAttachments->setText(email.attachment);
    tab.wgtTo->setText(email.to);

    m_data.wgtTabs->addTab(tab.page.get(), email.subject);

    if (row + 1 < static_cast<int>(emails.size())) {
      enableEmails(row + 1, 1);
    }
    else {
      if (!m_serverRoomLaunched) {
        m_serverRoomLaunched = true;
        commonData.eventSystem.fire(pEvent_t(new Event("launchServerRoom")));
      }
    }
  }

  m_data.wgtTabs->setCurrentIndex(1);
}

//===========================================
// FMailClient::setupEmailTab
//===========================================
void FMailClient::setupEmailTab() {
  auto& tab = m_data.emailTab;

  tab.page = makeQtObjPtr<QWidget>();
  tab.grid = makeQtObjPtr<QGridLayout>(tab.page.get());

  tab.wgtText = makeQtObjPtr<QTextBrowser>();

  QLabel* wgtFromLabel = new QLabel("From");
  QLabel* wgtDateLabel = new QLabel("Date");
  QLabel* wgtSubjectLabel = new QLabel("Subject");
  QLabel* wgtAttachmentsLabel = new QLabel("Attachments");
  QLabel* wgtToLabel = new QLabel("To");

  tab.wgtFrom = makeQtObjPtr<QLabel>("...");
  tab.wgtDate = makeQtObjPtr<QLabel>("...");
  tab.wgtSubject = makeQtObjPtr<QLabel>("...");
  tab.wgtAttachments = makeQtObjPtr<QLabel>("...");
  tab.wgtTo = makeQtObjPtr<QLabel>("...");

  QPushButton* wgtReply = new QPushButton("R̵̛e͞p͠ļ̧̀y̶̨");
  wgtReply->setDisabled(true);

  QPushButton* wgtForward = new QPushButton("̨Fo̕ŗ͡w̛a̸r̵͏͟d҉");
  wgtForward->setDisabled(true);

  QPushButton* wgtDelete = new QPushButton("̵D̴̀́e̴͟l͏et͠͝ȩ͟");
  wgtDelete->setDisabled(true);

  tab.grid->addWidget(tab.wgtText.get(), 6, 0, 1, 4);
  tab.grid->addWidget(wgtFromLabel, 0, 0);
  tab.grid->addWidget(wgtDateLabel, 1, 0);
  tab.grid->addWidget(wgtSubjectLabel, 2, 0);
  tab.grid->addWidget(wgtToLabel, 3, 0);
  tab.grid->addWidget(wgtAttachmentsLabel, 4, 0);
  tab.grid->addWidget(tab.wgtFrom.get(), 0, 1, 1, 3);
  tab.grid->addWidget(tab.wgtDate.get(), 1, 1, 1, 3);
  tab.grid->addWidget(tab.wgtSubject.get(), 2, 1, 1, 3);
  tab.grid->addWidget(tab.wgtTo.get(), 3, 1, 1, 3);
  tab.grid->addWidget(tab.wgtAttachments.get(), 4, 1, 1, 3);
  tab.grid->addWidget(wgtReply, 5, 1);
  tab.grid->addWidget(wgtForward, 5, 2);
  tab.grid->addWidget(wgtDelete, 5, 3);
}

//===========================================
// FMailClient::populateInbox
//===========================================
void FMailClient::populateInbox() {
  vector<const Email*> emails = getEmails(m_inboxState);
  QTableWidget& table = *m_data.inboxTab.wgtTable;

  table.clearContents();
  table.setRowCount(emails.size());

  int i = 0;
  for (auto email : emails) {
    constructInboxEntry(table, i, email->subjectGarbled, email->attachment != "",
      email->fromGarbled, email->dateGarbled);

    ++i;
  }

  table.resizeColumnToContents(3);
}

//===========================================
// FMailClient::setupInboxTab
//===========================================
void FMailClient::setupInboxTab() {
  auto& tab = m_data.inboxTab;

  tab.page = makeQtObjPtr<QWidget>();
  tab.vbox = makeQtObjPtr<QVBoxLayout>(tab.page.get());

  tab.wgtTable = makeQtObjPtr<QTableWidget>(0, 4);
  tab.wgtTable->setShowGrid(false);
  tab.wgtTable->setContextMenuPolicy(Qt::NoContextMenu);
  tab.wgtTable->setHorizontalHeaderLabels({"Subject", "", "From", "Date"});
  tab.wgtTable->horizontalHeaderItem(1)
    ->setIcon(QIcon(config::dataPath("youve_got_mail/attachment.png").c_str()));
  tab.wgtTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  tab.wgtTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  tab.wgtTable->verticalHeader()->setVisible(false);
  connect(tab.wgtTable.get(), SIGNAL(cellDoubleClicked(int, int)), this,
    SLOT(onCellDoubleClick(int, int)));

  populateInbox();
  enableEmails(0, 1);

  tab.vbox->addWidget(tab.wgtTable.get());

  m_data.wgtTabs->addTab(tab.page.get(), "larrym@apex.com");
}

//===========================================
// FMailClient::cleanUp
//===========================================
void FMailClient::cleanUp() {
  DBG_PRINT("FMailClient::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);

  commonData.eventSystem.forget(m_larryKilledId);
  commonData.eventSystem.forget(m_serversDestroyedId);
}

//===========================================
// FMailClient::~FMailClient
//===========================================
FMailClient::~FMailClient() {
  DBG_PRINT("FMailClient::~FMailClient\n");
}
