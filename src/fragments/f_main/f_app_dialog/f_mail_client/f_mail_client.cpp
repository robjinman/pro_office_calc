#include <QPushButton>
#include <QHeaderView>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_mail_client/f_mail_client.hpp"
#include "fragments/f_main/f_app_dialog/f_mail_client/f_mail_client_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"


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

const std::array<Email, 6> EMAILS = {{
  {
    "Potentially dangerous app - remove ASAP",
    "Alan Shand",
    "devteam",
    "13/03/95 13:06:44",

    "Potentially dangerous app - remove ASAP",
    "Alan Shand",
    "13/03/95 13:06:44",

    // Message body
    //
    "This is an urgent request to retract validation of Pro Office Calculator due to potentially "
    "dangerous behaviour not uncovered during initial testing. I've had Brian and his team look "
    "into this. It's clearly doing something odd, but they're unsure what.\n\n"

    "Please ensure this application does NOT ship with the upcoming release of Apex OS.\n\n"

    "Alan Shand,\n"
    "Chief QA Officer,\n"
    "Apex Systems\n"
  },
  {
    "Quick question",
    "Alan Shand",
    "devteam",
    "13/03/95 13:08:17",

    "Q̴̡̐̉u̵̻͑į̵̌̀c̸͍̆̒ḵ̶̛͊ ̸̲̽q̷̱̹͋ú̴̞ë̶̢̛̯́ŝ̷͘ͅt̷̢̅i̶̧̿o̵͓͋̈́n̵͖̄͂͜",
    "A̴̫͘l̴̲̊a̵͈̽͊n̴̨̥̂͐ ̵̛̱̓S̷̱̾h̷̼̩͐a̶̗̯̅͑n̴̨͈͗̈d̵͎̂̒",
    "1̴̙̽͊3̵̬͕̉/̷̪̉0̸̝͛̕3̷̖͔̄͝/̶̲̒͊9̵̮̗͐5̴̟̕ ̶̺̠̓1̷͎͍͊3̷̭̔̚:̶͓͗0̸͈͇̾̉8̵̧̦̂͠:̷͎̺̋1̷̲͊7̶͉̫̔͌",

    // Message body
    //
    "Is Rob Jinman one of our ex-contractors?\n\n"

    "Alan Shand,\n"
    "Chief QA Officer,\n"
    "Apex Systems\n"
  },
  {
    "Re: Quick question",
    "Paul Gresham",
    "devteam",
    "13/03/95 13:06:44",

    "R̴̩̄̚e̵͖̽͜:̵̒͜ ̵̢̺͌Q̵̦̲̑͌u̸̡̫͂ī̵̬̖̕c̶͇̍k̸̲̳̔͘ ̶͓̳̚q̶͕̺̈́́u̷͔̓̚ȩ̷̟̃̚s̵̫̪͛t̸̨̞́i̵̬͓͌ȯ̷̦͓̃ṇ̸̟̚",
    "Ṕ̶̱ạ̸̘̕u̴̫̻͐͝l̸̻̈́́ ̷̆̆ͅG̵̙̼̈̋r̸͔̉e̶͊̍ͅs̴̙͑h̵͕̗̿͋a̸̞͐m̴̘̚",
    "1̷̙̍̕3̷͈̥̓̋/̸͎̤͠0̴̬̪͑̀3̴͍̰̎/̷͙̔̄͜9̴̰̘̇̊5̸̗̯̎̕ ̵̖̊̉1̸̼̾͆3̸̥̳̚:̸̢̭͛͠0̷͓̗͒̚6̷̩̀͜:̴͔̐͛4̸̙̳̊̌4̷͚̈",

    // Message body
    //
    "Hi Alan,\n\n"

    "Yes, he one day dropped off the radar and we haven't seen him since - never even got his last "
    "invoice IIRC.\n\n"

    "Paul Gresham,\n"
    "Systems Architect,\n"
    "Apex Systems\n"
  },
  {
    "Latest regarding Pro Office Calc",
    "Brian Williams",
    "devteam",
    "15/03/95 11:20:09",

    "Ḽ̶̓͘à̷̬͜ṫ̶̙̑e̶͓̅s̷̙̈́̉t̸̲͉̓̚ ̸̨̜̏̈́r̵̥͂͝ë̸̡͕́g̸̮͋ã̶̟͇͒r̶̢̉̚d̷̹͌i̷̠̬͒̾n̶̛̗̎g̶̺͐ ̶̼͎̈̓P̶̡͕̂͌ṟ̷̺͂̕ȏ̷͚̔ ̷̗̎Ō̶͙̹f̵̮̫̿̐f̵͙͂͝î̴̗͗c̸̲̽̀e̸̮͓̒ ̷̳̮̋̾C̷̩̯̏ä̶̛͎͈́l̸̠̑͝c̶̛͎̔",
    "B̶̧̋̎ŗ̷͝ị̷̻̈́͝ā̵͖̲n̵̛̖͋ ̶̙̺̓͌W̵̝̚̚i̵̻͛l̸̛̙̑͜l̶͕̈̒i̷̖͇̅a̷̾̈ͅm̷͕̘̕ș̵̒",
    "1̷̻̪̈͛5̷̻͇̏/̸̭̊0̷̩̊3̵̤̂̂/̷̥͕͋̿9̵͍̎͒5̸̼͍̽ ̸̧̈́̕1̵̲̎1̶̘͐̉:̸̳̘͘2̴̧̳̐̏0̷̦̂:̸̳͚̍̒0̵̯͋̾9̷̨̧͂͝",

    // Message body
    //
    "Alan, it's using library functions and system calls that weren't implemented at the time the "
    "app was submitted. That doesn't make any sense.\n\n"

    "Brian Williams,\n"
    "Lead Kernel Developer,\n"
    "Apex Systems\n",

    "syscalls.txt"
  },
  {
    "re: Latest regarding Pro Office Calc",
    "Michael Considine",
    "devteam",
    "15/03/95 11:55:12",

    "ŕ̶̰̍ę̸̩̕:̵̯̣͌͂ ̵̯̖̈͛L̷̥͎͑a̴͇̭̕t̸͍̟͝͠e̴̛̤̒s̶͙̝͋t̵̼̐ ̸̖̪̓r̸̫͒̃e̶̦̅͠g̶͔͛͜â̶͇̑r̶̬̈́ḓ̵̃i̷̬͇̍̐ṋ̶͋g̶̣͇̃̑ ̷̱̓̐P̶͚̐̈r̴̙̼̎ȯ̶̥ ̷̹̽̿Õ̶̬͖͘ḟ̷̖͔̾f̴͍̘̌͊ī̵̧̉c̴̩̜͋̑ẻ̸̳ ̸̧̀C̷͍̈â̶̡̗͝l̶̠̞̈́̅c̷̻͑͝",
    "M̴̲̺͒̾i̶̻̜͑c̷͇͐́h̸̪͔̍́a̷̺͠ȅ̷̲̗̆l̵͙̅͠ ̷͙̩͆̕C̸̨̭̿͘o̵̮̖͌n̶͙͂s̶̘̈́̚͜ī̷̪̺ḏ̷͕́̂į̷̆n̶̡̓͋ë̵̫",
    "1̴͚͑͝5̸̢͕́/̵̹̒0̶̭̎3̶̹͍͊͐/̷̡̌́9̴̰̼͑͠5̸͉̀ ̴̡̝̋̆1̶̣̓1̷̱͉͒:̷͉́̄5̴̱̝͋͋5̵͓̍͜:̵͎͔̽̈1̷̪̜̑2̵̯̃́",

    // Message body
    //
    "We hadn't even thought of those functions yet. How is that possible?\n\n"

    "Michael Considine,\n"
    "Senior Kernel Developer,\n"
    "Apex Systems\n"
  },
  {
    "re: re: Latest regarding Pro Office Calc",
    "Brian Williams",
    "devteam",
    "15/03/95 12:01:19",

    "r̶̘͐e̸̱͙͌̌:̴͍̭̎ ̴̬̋r̷̪͆ê̴̡͕:̶͉̍ ̶͇̈́L̷̛̳ą̵̪͋ť̶̮̯ḙ̵͑̄s̵̗̈́́t̴̰͖̓́ ̶͈̜̂r̶̫̖͒e̶̛͔g̵̰̠̈ả̸̩̣̕r̴̛̩̟̀d̶̖̚i̷̝̒̊n̸̥̈̂ğ̷̻ ̵͉̊̏P̷͈̅r̸̩̝̓o̸͇̞̊͌ ̵̩̅͝O̴͚̔͋f̷̱̪͛̈́f̷̯̈́͆i̸̠̋ͅc̷̫̫̿ę̶͝ ̸̭̒̉C̴̻͊̊á̴̟͈͝l̷̰̐͆c̶̛̰̑ͅ",
    "B̸̀ͅr̵̦̫͌i̵̫̟͌̊á̵͎̪̚n̴̝̟̈́ ̸̨͊̚W̸͚̰͐ǐ̵̘̆ĺ̸̞̍l̵̺̖̔̚i̵͇̯͂̄a̷͓̜̿m̴͇̗͂̊s̵̘̹͗̒",
    "1̶̗̱̒5̸̙̈́͛/̶͎̋͠0̷̹̮̚3̵̧̞̌̐/̸͍̋̇9̷̨͊͠5̴̝̘̉͆ ̶͈̳̄1̷̦̱̃̉2̵̖̰̈́:̴̗̍̽0̴̤̠͐1̵͕͆:̸̯̜̂͘1̶̡͚̐9̸̘̒͘",

    // Message body
    //
    "It's not.\n\n"

    "Brian Williams,\n"
    "Lead Kernel Developer,\n"
    "Apex Systems\n"
  }
}};

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
// FMailClient::enableEmail
//===========================================
void FMailClient::enableEmail(int idx) {
  auto& email = EMAILS[idx];
  auto& inbox = m_data.inboxTab;

  enableRow(*inbox.wgtTable, idx);
  inbox.wgtTable->item(idx, 0)->setText(email.subject);
  if (email.attachment != "") {
    inbox.wgtTable->item(idx, 1)->setText("✓");
  }
  inbox.wgtTable->item(idx, 2)->setText(email.from);
  inbox.wgtTable->item(idx, 3)->setText(email.date);
}

//===========================================
// FMailClient::onCellDoubleClick
//===========================================
void FMailClient::onCellDoubleClick(int row, int col) {
  auto& email = EMAILS[row];
  auto& tab = m_data.emailTab;
  auto& inbox = m_data.inboxTab;

  if (inbox.wgtTable->item(row, col)->flags() & Qt::ItemIsEnabled) {
    m_data.wgtTabs->setCurrentIndex(1);

    tab.wgtText->setText(email.body);
    tab.wgtFrom->setText(email.from);
    tab.wgtSubject->setText(email.subject);
    tab.wgtAttachments->setText(email.attachment);
    tab.wgtTo->setText(email.to);

    m_data.wgtTabs->addTab(tab.page.get(), email.subject);

    if (row + 1 < static_cast<int>(EMAILS.size())) {
      enableEmail(row + 1);
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
  QLabel* wgtSubjectLabel = new QLabel("Subject");
  QLabel* wgtAttachmentsLabel = new QLabel("Attachments");
  QLabel* wgtToLabel = new QLabel("To");

  tab.wgtFrom = makeQtObjPtr<QLabel>("...");
  tab.wgtSubject = makeQtObjPtr<QLabel>("...");
  tab.wgtAttachments = makeQtObjPtr<QLabel>("...");
  tab.wgtTo = makeQtObjPtr<QLabel>("...");

  QPushButton* wgtReply = new QPushButton("R̵̛e͞p͠ļ̧̀y̶̨");
  wgtReply->setDisabled(true);

  QPushButton* wgtForward = new QPushButton("̨Fo̕ŗ͡w̛a̸r̵͏͟d҉");
  wgtForward->setDisabled(true);

  QPushButton* wgtDelete = new QPushButton("̵D̴̀́e̴͟l͏et͠͝ȩ͟");
  wgtDelete->setDisabled(true);

  tab.grid->addWidget(tab.wgtText.get(), 5, 0, 1, 4);
  tab.grid->addWidget(wgtFromLabel, 0, 0);
  tab.grid->addWidget(wgtSubjectLabel, 1, 0);
  tab.grid->addWidget(wgtToLabel, 2, 0);
  tab.grid->addWidget(wgtAttachmentsLabel, 3, 0);
  tab.grid->addWidget(tab.wgtFrom.get(), 0, 1, 1, 3);
  tab.grid->addWidget(tab.wgtSubject.get(), 1, 1, 1, 3);
  tab.grid->addWidget(tab.wgtTo.get(), 2, 1, 1, 3);
  tab.grid->addWidget(tab.wgtAttachments.get(), 3, 1, 1, 3);
  tab.grid->addWidget(wgtReply, 4, 1);
  tab.grid->addWidget(wgtForward, 4, 2);
  tab.grid->addWidget(wgtDelete, 4, 3);
}

//===========================================
// FMailClient::setupInboxTab
//===========================================
void FMailClient::setupInboxTab() {
  auto& tab = m_data.inboxTab;

  tab.page = makeQtObjPtr<QWidget>();
  tab.vbox = makeQtObjPtr<QVBoxLayout>(tab.page.get());

  tab.wgtTable = makeQtObjPtr<QTableWidget>(6, 4);
  tab.wgtTable->setShowGrid(false);
  tab.wgtTable->setContextMenuPolicy(Qt::NoContextMenu);
  tab.wgtTable->setHorizontalHeaderLabels({"Subject", "", "From", "Date"});
  tab.wgtTable->horizontalHeaderItem(1)->setIcon(QIcon("data/youve_got_mail/attachment.png"));
  tab.wgtTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  tab.wgtTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  tab.wgtTable->verticalHeader()->setVisible(false);
  connect(tab.wgtTable.get(), SIGNAL(cellDoubleClicked(int, int)), this,
    SLOT(onCellDoubleClick(int, int)));

  int i = 0;
  for (auto& email : EMAILS) {
    constructInboxEntry(*tab.wgtTable, i, email.subjectGarbled, email.attachment != "",
      email.fromGarbled, email.dateGarbled);

    ++i;
  }

  enableEmail(0);

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
}

//===========================================
// FMailClient::~FMailClient
//===========================================
FMailClient::~FMailClient() {
  DBG_PRINT("FMailClient::~FMailClient\n");
}
