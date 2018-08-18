#include <chrono>
#include <regex>
#include <QHeaderView>
#include <QLineEdit>
#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragments/f_main/f_app_dialog/f_doomsweeper/f_doomsweeper.hpp"
#include "fragments/f_main/f_app_dialog/f_doomsweeper/f_doomsweeper_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_doomsweeper/object_factory.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/events.hpp"
#include "raycast/render_system.hpp"
#include "utils.hpp"
#include "update_loop.hpp"
#include "app_config.hpp"
#include "state_ids.hpp"


using std::string;


//===========================================
// FDoomsweeper::FDoomsweeper
//===========================================
FDoomsweeper::FDoomsweeper(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FDoomsweeper", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FDoomsweeper::FDoomsweeper\n");
}

//===========================================
// FDoomsweeper::reload
//===========================================
void FDoomsweeper::reload(const FragmentSpec&) {
  DBG_PRINT("FDoomsweeper::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  if (m_data.stackedLayout) {
    delete m_data.stackedLayout.release();
  }

  m_data.stackedLayout = makeQtObjPtr<QStackedLayout>();

  setLayout(m_data.stackedLayout.get());

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  setupRaycastPage();
  setupHighScorePage();

  m_data.stackedLayout->setCurrentIndex(0);

  m_hLevelComplete = commonData.eventSystem.listen("doomsweeper/levelComplete",
    [this](const Event&) {

    m_data.stackedLayout->setCurrentIndex(1);
  });
}

//===========================================
// FDoomsweeper::setupRaycastPage
//===========================================
void FDoomsweeper::setupRaycastPage() {
  auto& page = m_data.raycastPage;

  page.wgtRaycast = makeQtObjPtr<RaycastWidget>(commonData.appConfig, commonData.eventSystem);

  auto& rootFactory = page.wgtRaycast->rootFactory();
  auto& timeService = page.wgtRaycast->timeService();
  auto& entityManager = page.wgtRaycast->entityManager();

  auto factory = new doomsweeper::ObjectFactory(rootFactory, entityManager, timeService);

  page.wgtRaycast->rootFactory().addFactory(pGameObjectFactory_t(factory));
  page.wgtRaycast->initialise(commonData.appConfig.dataPath("doomsweeper/map.svg"));

  page.gameLogic.reset(new doomsweeper::GameLogic(commonData.eventSystem,
    entityManager, page.wgtRaycast->rootFactory(), *factory, timeService));

  m_hSetup = commonData.eventSystem.listen("doomsweeper/minesweeperSetupComplete",
    [=](const Event& e_) {

    auto& e = dynamic_cast<const doomsweeper::MinesweeperSetupEvent&>(e_);
    m_initFuture = m_data.raycastPage.gameLogic->initialise(e.mineCoords);

    DBG_PRINT("Initialising game logic...\n");

    commonData.updateLoop.add([this]() -> bool {
      return waitForInit();
    }, []() {
      DBG_PRINT("DONE initialising game logic\n");
    });
  });

  m_data.stackedLayout->addWidget(page.wgtRaycast.get());
}

//===========================================
// constructTableItem
//===========================================
static QTableWidgetItem* constructTableItem(const QString& text) {
  Qt::ItemFlags disableFlags = Qt::ItemIsSelectable | Qt::ItemIsEditable;

  QTableWidgetItem* item = new QTableWidgetItem(text);
  item->setFlags(item->flags() & ~disableFlags);

  return item;
}

//===========================================
// constructNameItem
//===========================================
static QWidget* constructNameItem(const QString& text) {
  QLineEdit* item = new QLineEdit(text);

  QPalette p = item->palette();
  p.setColor(QPalette::Base, Qt::green);
  item->setPalette(p);

  item->setToolTip("Enter your name to continue");

  return item;
}

//===========================================
// FDoomsweeper::setupHighScorePage
//===========================================
void FDoomsweeper::setupHighScorePage() {
  auto& page = m_data.highScorePage;

  page.widget = makeQtObjPtr<QWidget>();
  page.vbox = makeQtObjPtr<QVBoxLayout>();

  page.widget->setLayout(page.vbox.get());

  page.wgtLabel = makeQtObjPtr<QLabel>("New high score!");

  page.wgtTable = makeQtObjPtr<QTableWidget>(4, 2);

  page.wgtTable->setShowGrid(true);
  page.wgtTable->setContextMenuPolicy(Qt::NoContextMenu);
  page.wgtTable->setHorizontalHeaderLabels({"Name", "Score"});
  page.wgtTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  page.wgtTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  page.wgtTable->verticalHeader()->setVisible(true);

  page.wgtTable->setItem(0, 0, constructTableItem("Dave Smith"));
  page.wgtTable->setItem(0, 1, constructTableItem("4412"));

  auto nameItem = constructNameItem("ENTER YOUR NAME");
  page.wgtTable->setCellWidget(1, 0, nameItem);

  auto scoreItem = constructTableItem("4052");
  scoreItem->setBackground(Qt::green);
  page.wgtTable->setItem(1, 1, scoreItem);

  page.wgtTable->setItem(2, 0, constructTableItem("Claire Pilch"));
  page.wgtTable->setItem(2, 1, constructTableItem("3787"));

  page.wgtTable->setItem(3, 0, constructTableItem("Herman Lewis"));
  page.wgtTable->setItem(3, 1, constructTableItem("3110"));

  page.wgtContinue = makeQtObjPtr<QPushButton>("Continue");
  page.wgtContinue->setToolTip("Enter your name to continue");
  page.wgtContinue->setDisabled(true);

  page.vbox->addWidget(page.wgtLabel.get());
  page.vbox->addWidget(page.wgtTable.get());
  page.vbox->addWidget(page.wgtContinue.get());

  m_data.stackedLayout->addWidget(page.widget.get());

  connect(nameItem, SIGNAL(textChanged(QString)), this, SLOT(onTableEdit()));
  connect(page.wgtContinue.get(), SIGNAL(clicked()), this, SLOT(onContinueClick()));
}

//===========================================
// FDoomsweeper::onTableEdit
//===========================================
void FDoomsweeper::onTableEdit() {
  const int MIN_SZ = 5;
  const int MAX_SZ = 28;

  auto& page = m_data.highScorePage;

  auto item = dynamic_cast<const QLineEdit*>(page.wgtTable->cellWidget(1, 0));
  m_playerName = item->text().toStdString();

  std::regex rxName{"^[a-zA-Z]+(?:\\s[a-zA-Z]+)+$"};
  std::smatch m;

  if (m_playerName != "ENTER YOUR NAME"
    && m_playerName.size() >= MIN_SZ && m_playerName.size() <= MAX_SZ
    && std::regex_match(m_playerName, m, rxName)) {

    page.wgtContinue->setDisabled(false);
  }
  else {
    page.wgtContinue->setDisabled(true);
  }
}

//===========================================
// FDoomsweeper::onContinueClick
//===========================================
void FDoomsweeper::onContinueClick() {
  commonData.eventSystem.fire(pEvent_t(new SetConfigParamEvent{"player-name", m_playerName}));
  commonData.eventSystem.fire(pEvent_t(new RequestStateChangeEvent{ST_T_MINUS_TWO_MINUTES}));
}

//===========================================
// FDoomsweeper::waitForInit
//===========================================
bool FDoomsweeper::waitForInit() {
  auto status = m_initFuture.wait_for(std::chrono::milliseconds(0));

  if (status == std::future_status::ready) {
    m_initFuture.get();

    m_data.raycastPage.wgtRaycast->start();
    return false;
  }

  return true;
}

//===========================================
// FDoomsweeper::cleanUp
//===========================================
void FDoomsweeper::cleanUp() {
  DBG_PRINT("FDoomsweeper::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
}

//===========================================
// FDoomsweeper::~FDoomsweeper
//===========================================
FDoomsweeper::~FDoomsweeper() {
  DBG_PRINT("FDoomsweeper::~FDoomsweeper\n");
}
