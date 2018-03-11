#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_procalc_setup/f_procalc_setup.hpp"
#include "fragments/f_main/f_app_dialog/f_procalc_setup/f_procalc_setup_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"


using std::set;
using making_progress::GameLogic;


//===========================================
// addHeaderItem
//===========================================
static void addHeaderItem(QListWidget& wgtList, const QString& text) {
  QListWidgetItem* item = new QListWidgetItem(text);

  QFont bold = item->font();
  bold.setBold(true);

  item->setFont(bold);

  Qt::ItemFlags on = Qt::NoItemFlags;
  Qt::ItemFlags off = Qt::ItemIsSelectable;

  item->setFlags(item->flags() | on);
  item->setFlags(item->flags() & ~off);

  wgtList.addItem(item);
}

//===========================================
// addSpaceItem
//===========================================
static void addSpaceItem(QListWidget& wgtList) {
  addHeaderItem(wgtList, "");
}

//===========================================
// FProcalcSetup::FProcalcSetup
//===========================================
FProcalcSetup::FProcalcSetup(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FProcalcSetup", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FProcalcSetup::FProcalcSetup\n");
}

//===========================================
// FProcalcSetup::reload
//===========================================
void FProcalcSetup::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FProcalcSetup::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  m_data.stackedLayout = makeQtObjPtr<QStackedLayout>(this);
  setLayout(m_data.stackedLayout.get());

  setupPage1();
  setupPage2();

  m_data.stackedLayout->addWidget(m_data.page1.widget.get());
  m_data.stackedLayout->addWidget(m_data.page2.widget.get());
}

//===========================================
// FProcalcSetup::addCheckableItem
//===========================================
void FProcalcSetup::addCheckableItem(QListWidget& wgtList, const QString& text, buttonId_t btnId) {
  QListWidgetItem* item = new QListWidgetItem(text);

  Qt::ItemFlags on = Qt::ItemIsUserCheckable;
  Qt::ItemFlags off = Qt::ItemIsSelectable;

  item->setFlags(item->flags() | on);
  item->setFlags(item->flags() & ~off);

  item->setCheckState(Qt::Unchecked);

  m_featureIndices[btnId] = wgtList.count();
  wgtList.addItem(item);
}

//===========================================
// FProcalcSetup::populateListWidget
//===========================================
void FProcalcSetup::populateListWidget() {
  QListWidget& wgtList = *m_data.page1.wgtList;

  addHeaderItem(wgtList, "Essential features");
  addCheckableItem(wgtList, "Equals button", BTN_EQUALS);
  addSpaceItem(wgtList);
  addHeaderItem(wgtList, "Recommended extras");
  addCheckableItem(wgtList, "Number 0", BTN_ZERO);
  addCheckableItem(wgtList, "Number 2", BTN_TWO);
  addCheckableItem(wgtList, "Number 4", BTN_FOUR);
  addCheckableItem(wgtList, "Number 7", BTN_SEVEN);
  addCheckableItem(wgtList, "Number 6", BTN_SIX);
  addCheckableItem(wgtList, "Number 8", BTN_EIGHT);
  addCheckableItem(wgtList, "Addition operator", BTN_PLUS);
  addCheckableItem(wgtList, "Subtraction operator", BTN_MINUS);
  addCheckableItem(wgtList, "Multiplication operator", BTN_TIMES);
  addCheckableItem(wgtList, "Division operator", BTN_DIVIDE);
  addCheckableItem(wgtList, "Clear button", BTN_CLEAR);
  addSpaceItem(wgtList);
  addHeaderItem(wgtList, "Advanced features");
  addCheckableItem(wgtList, "Number 1", BTN_ONE);
  addCheckableItem(wgtList, "Number 5", BTN_FIVE);
  addSpaceItem(wgtList);
  addHeaderItem(wgtList, "Experimental features");
  addCheckableItem(wgtList, "Number 3", BTN_THREE);
  addCheckableItem(wgtList, "Number 9", BTN_NINE);
  addCheckableItem(wgtList, "Decimal point", BTN_POINT);
}

//===========================================
// FProcalcSetup::setupPage1
//===========================================
void FProcalcSetup::setupPage1() {
  auto& page = m_data.page1;

  page.widget = makeQtObjPtr<QWidget>();

  QVBoxLayout* vbox = new QVBoxLayout;
  page.widget->setLayout(vbox);

  QPixmap icon("data/warning.png");

  QLabel* wgtIcon = new QLabel;
  wgtIcon->setPixmap(icon);
  wgtIcon->setMaximumWidth(50);
  wgtIcon->setMaximumHeight(50);
  wgtIcon->setScaledContents(true);

  QLabel* wgtCaption = new QLabel("Selecting more features will result in a longer setup time");
  wgtCaption->setWordWrap(true);

  QHBoxLayout* topHBox = new QHBoxLayout;
  topHBox->addWidget(wgtIcon);
  topHBox->addWidget(wgtCaption);

  page.wgtList = makeQtObjPtr<QListWidget>();

  page.wgtNext = makeQtObjPtr<QPushButton>("Next");

  QHBoxLayout* bottomHBox = new QHBoxLayout;
  bottomHBox->addStretch(0);
  bottomHBox->addWidget(page.wgtNext.get());

  vbox->addLayout(topHBox);
  vbox->addWidget(page.wgtList.get());
  vbox->addLayout(bottomHBox);

  populateListWidget();

  connect(page.wgtNext.get(), SIGNAL(clicked()), this, SLOT(onNextClick()));
}

//===========================================
// FProcalcSetup::setupPage2
//===========================================
void FProcalcSetup::setupPage2() {
  auto& page = m_data.page2;

  page.widget = makeQtObjPtr<QWidget>();

  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->setSpacing(0);
  vbox->setContentsMargins(0, 0, 0, 0);

  page.widget->setLayout(vbox);

  page.wgtRaycast = makeQtObjPtr<RaycastWidget>(commonData.eventSystem);
  vbox->addWidget(page.wgtRaycast.get());

  page.wgtRaycast->initialise("data/maps/procalc_setup.svg");

  QDialog& dialog = parentFrag<QDialog>();

  page.gameLogic = makeQtObjPtr<GameLogic>(dialog, commonData.eventSystem,
    page.wgtRaycast->entityManager());
}

//===========================================
// FProcalcSetup::onNextClick
//===========================================
void FProcalcSetup::onNextClick() {
  m_data.stackedLayout->setCurrentIndex(1);

  set<buttonId_t> features;
  for (auto it = m_featureIndices.begin(); it != m_featureIndices.end(); ++it) {
    buttonId_t feature = it->first;
    QListWidgetItem& item = *m_data.page1.wgtList->item(it->second);

    if (item.checkState() == Qt::Checked) {
      features.insert(feature);
    }
  }

  m_data.page2.gameLogic->setFeatures(features);
}

//===========================================
// FProcalcSetup::cleanUp
//===========================================
void FProcalcSetup::cleanUp() {
  DBG_PRINT("FProcalcSetup::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);
}

//===========================================
// FProcalcSetup::~FProcalcSetup
//===========================================
FProcalcSetup::~FProcalcSetup() {
  DBG_PRINT("FProcalcSetup::~FProcalcSetup\n");
}
