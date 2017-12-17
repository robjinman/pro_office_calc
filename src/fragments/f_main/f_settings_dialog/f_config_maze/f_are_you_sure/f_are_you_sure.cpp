#include <regex>
#include <random>
#include <cassert>
#include <algorithm>
#include <QPixmap>
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_are_you_sure/f_are_you_sure.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_are_you_sure/f_are_you_sure_spec.hpp"


using std::string;


const int NUM_QUESTIONS = 5;

static std::random_device rd;
static std::mt19937 randEngine(rd());


//===========================================
// FAreYouSure::Template::generate
//===========================================
string FAreYouSure::Template::generate(const TemplateMap& templates, int maxDepth) const {
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
// FAreYouSure::FAreYouSure
//===========================================
FAreYouSure::FAreYouSure(Fragment& parent_, FragmentData& parentData_)
  : QWidget(nullptr),
    Fragment("FAreYouSure", parent_, parentData_, m_data) {

  auto& parentData = parentFragData<FConfigMazeData>();

  m_origParentData.layout = parentData.vbox.get();
}

//===========================================
// FAreYouSure::rebuild
//===========================================
void FAreYouSure::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<FConfigMaze>();
  auto& parentData = parentFragData<FConfigMazeData>();

  auto& spec = dynamic_cast<const FAreYouSureSpec&>(spec_);

  m_data.vbox.reset(new QVBoxLayout(this));
  m_data.hbox.reset(new QHBoxLayout());

  m_data.wgtIcon.reset(new QLabel());
  m_data.wgtIcon->setPixmap(QPixmap("data/warning.png"));

  m_data.wgtPrompt.reset(new QLabel());
  m_data.wgtPrompt->setWordWrap(true);

  m_data.wgtYes.reset(new QPushButton("Yes"));
  m_data.wgtNo.reset(new QPushButton("No"));

  m_data.wgtFinalYes.reset(new EvasiveButton("Click me!"));

  m_data.hbox->addWidget(m_data.wgtNo.get());
  m_data.hbox->addWidget(m_data.wgtYes.get());

  m_data.vbox->addWidget(m_data.wgtIcon.get());
  m_data.vbox->addWidget(m_data.wgtPrompt.get());
  m_data.vbox->addLayout(m_data.hbox.get());

  connect(m_data.wgtYes.get(), SIGNAL(pressed()), this, SLOT(onYesClick()));
  connect(m_data.wgtNo.get(), SIGNAL(pressed()), this, SLOT(onNoClick()));

  m_data.outerVbox.reset(new QVBoxLayout);
  m_data.outerVbox->addWidget(this);

  parent.setLayout(nullptr);
  parent.setLayout(m_data.outerVbox.get());

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

  Fragment::rebuild(spec_);

  restart();
}

//===========================================
// FAreYouSure::restart
//===========================================
void FAreYouSure::restart() {
  m_count = 0;
  m_data.hbox->removeWidget(m_data.wgtFinalYes.get());
  m_data.hbox->addWidget(m_data.wgtYes.get());
  nextQuestion();
}

//===========================================
// FAreYouSure::nextQuestion
//===========================================
void FAreYouSure::nextQuestion() {
  if (m_count < NUM_QUESTIONS) {
    string question = m_templates.at("question").generate(m_templates, 6 + m_count);
    question.pop_back();
    question.push_back('?');

    m_data.wgtPrompt->setText(question.c_str());
  }
  else {
    m_data.wgtPrompt->setText("Are you sure?");
    m_data.hbox->removeWidget(m_data.wgtYes.get());
    m_data.hbox->addWidget(m_data.wgtFinalYes.get());
    m_data.wgtFinalYes->reset();
  }

  ++m_count;
}

//===========================================
// FAreYouSure::onYesClick
//===========================================
void FAreYouSure::onYesClick() {
  string question = m_data.wgtPrompt->text().toStdString();
  bool yesToContinue = numNegatives(question) % 2 == 0;

  if (yesToContinue) {
    nextQuestion();
  }
  else {
    restart();
  }
}

//===========================================
// FAreYouSure::onNoClick
//===========================================
void FAreYouSure::onNoClick() {
  string question = m_data.wgtPrompt->text().toStdString();
  bool yesToContinue = numNegatives(question) % 2 == 0;

  if (yesToContinue) {
    restart();
  }
  else {
    nextQuestion();
  }
}

//===========================================
// FAreYouSure::mouseMoveEvent
//===========================================
void FAreYouSure::mouseMoveEvent(QMouseEvent*) {
  m_data.wgtFinalYes->onMouseMove();
}

//===========================================
// FAreYouSure::cleanUp
//===========================================
void FAreYouSure::cleanUp() {
  if (m_origParentData.layout) {
    auto& parent = parentFrag<FConfigMaze>();
    parent.setLayout(m_origParentData.layout);
  }
}
