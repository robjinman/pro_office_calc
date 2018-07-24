#ifndef __PROCALC_FRAGMENTS_F_CONFIG_MAZE_ARE_YOU_SURE_WIDGET_HPP__
#define __PROCALC_FRAGMENTS_F_CONFIG_MAZE_ARE_YOU_SURE_WIDGET_HPP__


#include <map>
#include <QPushButton>
#include <QLabel>
#include <QStackedLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "evasive_button.hpp"
#include "qt_obj_ptr.hpp"


class AppConfig;

class AreYouSureWidget : public QWidget {
  Q_OBJECT

  public:
    AreYouSureWidget(const AppConfig& appConfig);

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

    class Template {
      public:
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

    QtObjPtr<QStackedLayout> m_pages;

    struct {
      QtObjPtr<QWidget> widget;
      QtObjPtr<QGridLayout> grid;
      QtObjPtr<QPushButton> wgtYes;
      QtObjPtr<QPushButton> wgtNo;
      QtObjPtr<QLabel> wgtPrompt;
      QtObjPtr<QLabel> wgtWarning;
    } m_page1;

    struct {
      QtObjPtr<QWidget> widget;
      QtObjPtr<QVBoxLayout> vbox;
      QtObjPtr<EvasiveButton> wgtProceed;
      QtObjPtr<QPushButton> wgtBackToSafety;
      QtObjPtr<QLabel> wgtPrompt;
      QtObjPtr<QLabel> wgtConsole;
    } m_page2;

    TemplateMap m_templates;
    int m_count;
};


#endif
