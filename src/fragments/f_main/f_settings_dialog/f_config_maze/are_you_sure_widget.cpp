#include <regex>
#include <random>
#include <cassert>
#include <algorithm>
#include <QPixmap>
#include "fragments/f_main/f_settings_dialog/f_config_maze/are_you_sure_widget.hpp"
#include "utils.hpp"
#include "app_config.hpp"


using std::string;


const int NUM_QUESTIONS = 8;

static std::mt19937 randEngine(randomSeed());


//===========================================
// AreYouSureWidget::Template::generate_
//===========================================
string AreYouSureWidget::Template::generate_(const TemplateMap& templates, const string& text,
  int maxDepth) const {

  if (maxDepth < 0) {
    return "";
  }

  std::regex rx("<(\\w+),(\\d+)-(\\d+)>");
  auto begin = std::sregex_iterator(text.begin(), text.end(), rx);
  auto end = std::sregex_iterator();

  string result = text;
  int offset = 0;

  for (auto it = begin; it != end; ++it) {
    std::smatch m = *it;
    string name = m.str(1);
    int minReps = std::atoi(m.str(2).c_str());
    int maxReps = std::atoi(m.str(3).c_str());

    std::uniform_int_distribution<int> randReps(minReps, maxReps);
    int reps = randReps(randEngine);

    string expanded;
    for (int rep = 0; rep < reps; ++rep) {
      string subresult = templates.at(name).generate(templates, maxDepth - 1);
      if (subresult.length() == 0 && minReps > 0) {
        return "";
      }

      expanded += subresult;
    }

    result.replace(m.position() + offset, m.length(), expanded);
    offset += static_cast<int>(expanded.length() - m.length());
  }

  if (result.back() != ' ') {
    result.push_back(' ');
  }

  return result;
}

//===========================================
// AreYouSureWidget::Template::generate
//===========================================
string AreYouSureWidget::Template::generate(const TemplateMap& templates, int maxDepth) const {
  string text = text1;

  if (text2.length() > 0) {
    std::uniform_int_distribution<int> flipCoin(0, 1);
    int coin = flipCoin(randEngine);

    text = (coin == 0 ? text1 : text2);
    const string& altText = (coin == 1 ? text1 : text2);

    string result = generate_(templates, text, maxDepth);
    if (result.length() == 0) {
      return generate_(templates, altText, maxDepth);
    }
    return result;
  }
  else {
    return generate_(templates, text, maxDepth);
  }
}

//===========================================
// numOccurrences
//===========================================
static int numOccurrences(const string& str, const string& substr) {
  int i = 0;
  auto it = str.begin();

  while (true) {
    it = std::search(it, str.end(), substr.begin(), substr.end());

    if (it == str.end()) {
      break;
    }
    else {
      ++i;
      ++it;
    }
  }

  return i;
}

//===========================================
// numNegatives
//===========================================
static inline int numNegatives(const string& str) {
  return numOccurrences(str, "abort") + numOccurrences(str, "not");
}

//===========================================
// AreYouSureWidget::AreYouSureWidget
//===========================================
AreYouSureWidget::AreYouSureWidget(const AppConfig& appConfig)
  : QWidget(nullptr) {

  setMouseTracking(true);

  m_pages = makeQtObjPtr<QStackedLayout>(this);

  // Page1
  //

  m_page1.widget = makeQtObjPtr<QWidget>();
  m_page1.widget->setMouseTracking(true);

  QFont font = appConfig.normalFont;
  font.setPixelSize(12);
  m_page1.widget->setFont(font);

  m_page1.grid = makeQtObjPtr<QGridLayout>();
  m_page1.widget->setLayout(m_page1.grid.get());

  QPixmap pixmap{appConfig.dataPath("common/images/warning.png").c_str()};
  m_page1.wgtWarning = makeQtObjPtr<QLabel>();
  m_page1.wgtWarning->setMouseTracking(true);
  m_page1.wgtWarning->setPixmap(pixmap);

  m_page1.wgtPrompt = makeQtObjPtr<QLabel>();
  m_page1.wgtPrompt->setWordWrap(true);
  m_page1.wgtPrompt->setMouseTracking(true);

  m_page1.wgtYes = makeQtObjPtr<QPushButton>("Yes");

  m_page1.wgtNo = makeQtObjPtr<QPushButton>("No");

  m_page1.grid->addWidget(m_page1.wgtWarning.get(), 0, 1);
  m_page1.grid->addWidget(m_page1.wgtPrompt.get(), 1, 0, 1, 3);
  m_page1.grid->addWidget(m_page1.wgtNo.get(), 2, 0);
  m_page1.grid->addWidget(m_page1.wgtYes.get(), 2, 2);

  connect(m_page1.wgtYes.get(), SIGNAL(clicked()), this, SLOT(onYesClick()));
  connect(m_page1.wgtNo.get(), SIGNAL(clicked()), this, SLOT(onNoClick()));

  m_pages->addWidget(m_page1.widget.get());

  // Page 2
  //

  m_page2.widget = makeQtObjPtr<QWidget>();
  m_page2.widget->setMouseTracking(true);

  m_page2.vbox = makeQtObjPtr<QVBoxLayout>();
  m_page2.widget->setLayout(m_page2.vbox.get());

  pixmap = QPixmap{appConfig.dataPath("common/images/console.png").c_str()};
  m_page2.wgtConsole = makeQtObjPtr<QLabel>();
  m_page2.wgtConsole->setMouseTracking(true);
  m_page2.wgtConsole->setPixmap(pixmap);
  m_page2.wgtConsole->setFixedSize(pixmap.size());

  m_page2.wgtPrompt = makeQtObjPtr<QLabel>("The admin console is for advanced users only. "
    "Enter at your own risk.");
  m_page2.wgtPrompt->setWordWrap(true);
  m_page2.wgtPrompt->setMouseTracking(true);

  m_page2.wgtBackToSafety = makeQtObjPtr<QPushButton>("Back to safety");
  m_page2.wgtBackToSafety->setMaximumWidth(120);

  m_page2.wgtProceed = makeQtObjPtr<EvasiveButton>("Proceed");
  m_page2.wgtProceed->setMaximumWidth(90);

  m_page2.vbox->addWidget(m_page2.wgtConsole.get());
  m_page2.vbox->addWidget(m_page2.wgtProceed.get());
  m_page2.vbox->addItem(new QSpacerItem(100, 20));
  m_page2.vbox->addWidget(m_page2.wgtPrompt.get());
  m_page2.vbox->addWidget(m_page2.wgtBackToSafety.get());

  m_page2.vbox->setAlignment(m_page2.wgtConsole.get(), Qt::AlignHCenter);
  m_page2.vbox->setAlignment(m_page2.wgtProceed.get(), Qt::AlignHCenter);

  connect(m_page2.wgtProceed.get(), SIGNAL(pressed()), this, SLOT(onFinalYesClick()));
  connect(m_page2.wgtBackToSafety.get(), SIGNAL(clicked()), this, SLOT(onFinalNoClick()));

  m_pages->addWidget(m_page2.widget.get());

  // Setup templates
  //

  m_templates["not"] = Template("not");
  m_templates["sureYou"] = Template("<not,0-3>sure you");
  m_templates["sureYouAre"] = Template("<sureYou,1-1>are");
  m_templates["sureYou_"] = Template("<sureYouAre,0-1><sureYou,1-1>");
  m_templates["sureYouWantTo"] = Template("<sureYou_,1-1>want to");
  m_templates["continue"] = Template("continue", "proceed");
  m_templates["verb"] = Template("<not,0-3><continue,1-1>", "<not,0-3>abort");
  m_templates["continuing"] = Template("continuing", "proceeding");
  m_templates["ing"] = Template("<not,0-3><continuing,1-1>", "<not,0-3>aborting");
  m_templates["continueTo"] = Template("<continue,1-1><ingTo,1-1>", "<continueTo,1-1><verbTo,1-1>");
  m_templates["verbTo"] = Template("<continueTo,1-1>", "abort <ingTo,1-1>");
  m_templates["ingTo"] = Template("<ingIng,1-1><continuing,1-1>to");
  m_templates["ingVerbIng"] = Template("<ingTo,1-1><verbIng,1-1>");
  m_templates["ingIng"] = Template("<ing,1-2><ingVerbIng,0-1>");
  m_templates["verbIng"] = Template("<verb,1-1><ingIng,1-1>");
  m_templates["verbVerb"] = Template("<verbTo,1-1><verb,1-1>");
  m_templates["verb_"] = Template("<verbIng,1-1>", "<verbVerb,1-1>");
  m_templates["question"] = Template("Are you sure you are <sureYouWantTo,1-1><verb_,1-1>");

  restart();
}

//===========================================
// AreYouSureWidget::restart
//===========================================
void AreYouSureWidget::restart() {
  m_count = 0;
  m_pages->setCurrentIndex(0);

  nextQuestion();
}

//===========================================
// AreYouSureWidget::nextQuestion
//===========================================
void AreYouSureWidget::nextQuestion() {
  if (m_count < NUM_QUESTIONS) {
    string question;

    if (m_count == 0) {
      question = "Are you sure you want to continue?";
    }
    else {
      while (question.length() == 0 || question.length() > 300) {
        question = m_templates.at("question").generate(m_templates, 6 + m_count);
        question.pop_back();
        question.push_back('?');
      }
    }

    DBG_PRINT((numNegatives(question) % 2 ? "N\n" : "Y\n"));

    m_page1.wgtPrompt->setText(question.c_str());
  }
  else {
    m_pages->setCurrentIndex(1);
    m_page2.wgtProceed->reset();
  }

  ++m_count;
}

//===========================================
// AreYouSureWidget::onYesClick
//===========================================
void AreYouSureWidget::onYesClick() {
  string question = m_page1.wgtPrompt->text().toStdString();
  bool yesToContinue = numNegatives(question) % 2 == 0;

  if (yesToContinue) {
    nextQuestion();
  }
  else {
    emit finished(false);
    restart();
  }
}

//===========================================
// AreYouSureWidget::onNoClick
//===========================================
void AreYouSureWidget::onNoClick() {
  string question = m_page1.wgtPrompt->text().toStdString();
  bool yesToContinue = numNegatives(question) % 2 == 0;

  if (yesToContinue) {
    emit finished(false);
    restart();
  }
  else {
    nextQuestion();
  }
}

//===========================================
// AreYouSureWidget::onFinalYesClick
//===========================================
void AreYouSureWidget::onFinalYesClick() {
  emit finished(true);
}

//===========================================
// AreYouSureWidget::onFinalNoClick
//===========================================
void AreYouSureWidget::onFinalNoClick() {
  emit finished(false);
  restart();
}

//===========================================
// AreYouSureWidget::mouseMoveEvent
//===========================================
void AreYouSureWidget::mouseMoveEvent(QMouseEvent*) {
  m_page2.wgtProceed->onMouseMove();
}
