#ifndef __PROCALC_FRAGMENTS_F_CONFIG_MAZE_ARE_YOU_SURE_WIDGET_HPP__
#define __PROCALC_FRAGMENTS_F_CONFIG_MAZE_ARE_YOU_SURE_WIDGET_HPP__


#include <memory>
#include <map>
#include <QPushButton>
#include <QLabel>
#include <QStackedLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include "evasive_button.hpp"


class AreYouSureWidget : public QWidget {
  Q_OBJECT

  public:
    AreYouSureWidget();

    void restart();

  signals:
    void finished(bool pass);

  protected:
    void mouseMoveEvent(QMouseEvent* event) override;

  private slots:
    void onYesClick();
    void onNoClick();
    void onFinalYesClick();
    void onFinalNoClick();

  private:
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
      std::string generate_(const TemplateMap& templates, const std::string& text,
        int maxDepth) const;

      std::string text1;
      std::string text2;
    };

    std::unique_ptr<QStackedLayout> m_pages;

    struct {
      std::unique_ptr<QWidget> widget;
      std::unique_ptr<QGridLayout> grid;
      std::unique_ptr<QPushButton> wgtYes;
      std::unique_ptr<QPushButton> wgtNo;
      std::unique_ptr<QLabel> wgtPrompt;
      std::unique_ptr<QLabel> wgtWarning;
    } m_page1;

    struct {
      std::unique_ptr<QWidget> widget;
      std::unique_ptr<QGridLayout> grid;
      std::unique_ptr<EvasiveButton> wgtYes;
      std::unique_ptr<QPushButton> wgtNo;
      std::unique_ptr<QLabel> wgtPrompt;
      std::unique_ptr<QLabel> wgtConsole;
    } m_page2;

    TemplateMap m_templates;
    int m_count;
};


#endif
