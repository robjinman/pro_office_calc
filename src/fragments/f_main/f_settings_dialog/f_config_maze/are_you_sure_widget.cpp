#include <regex>
#include <random>
#include <cassert>
#include <algorithm>
#include <QPixmap>
#include "fragments/f_main/f_settings_dialog/f_config_maze/are_you_sure_widget.hpp"
#include "event_system.hpp"
#include "utils.hpp"


using std::string;


const int NUM_QUESTIONS = 8;

static std::random_device rd;
static std::mt19937 randEngine(rd());


//===========================================
// AreYouSureWidget::Template::generate
//===========================================
string AreYouSureWidget::Template::generate(const TemplateMap& templates, int maxDepth) const {
  string text = text1;
  if (text2.length() > 0) {
    std::uniform_int_distribution<int> flipCoin(0, 1);
    int coin = flipCoin(randEngine);

    text = (coin == 0 ? text1 : text2);
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
    int reps = maxDepth > 0 ? randReps(randEngine) : minReps;

    string expanded;
    for (int rep = 0; rep < reps; ++rep) {
      expanded += templates.at(name).generate(templates, maxDepth - 1);
    }

    result.replace(m.position() + offset, m.length(), expanded);
    offset += expanded.length() - m.length();
  }

  if (result.back() != ' ') {
    result.push_back(' ');
  }

  return result;
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
AreYouSureWidget::AreYouSureWidget(EventSystem& eventSystem)
  : QWidget(nullptr),
    m_eventSystem(eventSystem) {

  m_grid.reset(new QGridLayout(this));

  m_wgtIcon.reset(new QLabel());
  m_wgtIcon->setPixmap(QPixmap("data/warning.png"));

  m_wgtPrompt.reset(new QLabel());
  m_wgtPrompt->setWordWrap(true);

  const int btnWidth = 1000;

  m_wgtYes.reset(new QPushButton("Yes"));
  m_wgtYes->setMaximumWidth(btnWidth);

  m_wgtNo.reset(new QPushButton("No"));
  m_wgtNo->setMaximumWidth(btnWidth);

  m_wgtFinalYes.reset(new EvasiveButton("Yes"));
  m_wgtFinalYes->setMaximumWidth(btnWidth);

  m_grid->addWidget(m_wgtIcon.get(), 0, 1);
  m_grid->addWidget(m_wgtPrompt.get(), 1, 0, 1, -1);
  m_grid->addWidget(m_wgtNo.get(), 2, 0);
  m_grid->addWidget(m_wgtYes.get(), 2, 2);

  connect(m_wgtYes.get(), SIGNAL(clicked()), this, SLOT(onYesClick()));
  connect(m_wgtNo.get(), SIGNAL(clicked()), this, SLOT(onNoClick()));
  connect(m_wgtFinalYes.get(), SIGNAL(pressed()), this, SLOT(onFinalYesClick()));

  m_templates["not"] = Template("not");
  m_templates["sureYou"] = Template("<not,0-3>sure you");
  m_templates["sureYouAre"] = Template("<sureYou,1-1>are");
  m_templates["sureYou_"] = Template("<sureYouAre,0-1><sureYou,1-1>");
  m_templates["sureYouWantTo"] = Template("<sureYou_,1-1>want to");
  m_templates["verb"] = Template("<not,0-3>continue", "<not,0-3>abort");
  m_templates["ing"] = Template("<not,0-3>continuing", "<not,0-3>aborting");
  m_templates["continueTo"] = Template("continue <ingTo,1-1>", "<continueTo,1-1><verbTo,1-1>");
  m_templates["verbTo"] = Template("<continueTo,1-1>", "abort <ingTo,1-1>");
  m_templates["ingTo"] = Template("<ingIng,1-1>continuing to");
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

  m_wgtFinalYes->hide();
  m_wgtYes->show();

  nextQuestion();
}

//===========================================
// AreYouSureWidget::nextQuestion
//===========================================
void AreYouSureWidget::nextQuestion() {
  if (m_count < NUM_QUESTIONS) {
    string question = m_templates.at("question").generate(m_templates, 6 + m_count);
    question.pop_back();
    question.push_back('?');

    DBG_PRINT((numNegatives(question) % 2 ? "N\n" : "Y\n"));

    m_wgtPrompt->setText(question.c_str());
  }
  else {
    m_wgtPrompt->setText("Are you sure?");

    m_wgtYes->hide();

    m_grid->addWidget(m_wgtFinalYes.get(), 2, 2);
    m_wgtFinalYes->show();
    m_wgtFinalYes->reset();
  }

  ++m_count;
}

//===========================================
// AreYouSureWidget::onYesClick
//===========================================
void AreYouSureWidget::onYesClick() {
  string question = m_wgtPrompt->text().toStdString();
  bool yesToContinue = numNegatives(question) % 2 == 0;

  if (yesToContinue) {
    nextQuestion();
  }
  else {
    m_eventSystem.fire("areYouSureFail");
    restart();
  }
}

//===========================================
// AreYouSureWidget::onNoClick
//===========================================
void AreYouSureWidget::onNoClick() {
  string question = m_wgtPrompt->text().toStdString();
  bool yesToContinue = numNegatives(question) % 2 == 0;

  if (yesToContinue) {
    m_eventSystem.fire("areYouSureFail");
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
  m_eventSystem.fire("areYouSurePass");
}

//===========================================
// AreYouSureWidget::mouseMoveEvent
//===========================================
void AreYouSureWidget::mouseMoveEvent(QMouseEvent*) {
  m_wgtFinalYes->onMouseMove();
}
