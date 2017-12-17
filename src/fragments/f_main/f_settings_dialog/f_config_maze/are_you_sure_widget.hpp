#ifndef __PROCALC_FRAGMENTS_ARE_YOU_SURE_WIDGET_HPP__
#define __PROCALC_FRAGMENTS_ARE_YOU_SURE_WIDGET_HPP__


#include <memory>
#include <map>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include "evasive_button.hpp"


class EventSystem;

class AreYouSureWidget : public QWidget {
  Q_OBJECT

  public:
    AreYouSureWidget(EventSystem& eventSystem);

  protected:
    void mouseMoveEvent(QMouseEvent* event) override;

  private slots:
    void onYesClick();
    void onNoClick();
    void onFinalYesClick();

  private:
    void restart();
    void nextQuestion();

    class Template;
    typedef std::map<std::string, Template> TemplateMap;

    struct Template {
      Template(const std::string& text1, const std::string& text2)
        : text1(text1),
          text2(text2) {}

      explicit Template(const std::string& text1)
        : text1(text1),
          text2("") {}

      Template() {}

      std::string generate(const TemplateMap& templates, int maxDepth = 10) const;

      std::string text1;
      std::string text2;
    };

    EventSystem& m_eventSystem;

    std::unique_ptr<QGridLayout> m_grid;
    std::unique_ptr<QPushButton> m_wgtYes;
    std::unique_ptr<QPushButton> m_wgtNo;
    std::unique_ptr<QLabel> m_wgtPrompt;
    std::unique_ptr<QLabel> m_wgtIcon;
    std::unique_ptr<EvasiveButton> m_wgtFinalYes;

    TemplateMap m_templates;
    int m_count;
};


#endif
