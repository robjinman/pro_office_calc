#ifndef __PROCALC_FRAGMENTS_F_ARE_YOU_SURE_HPP__
#define __PROCALC_FRAGMENTS_F_ARE_YOU_SURE_HPP__


#include <memory>
#include <map>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "fragment.hpp"
#include "evasive_button.hpp"


struct FAreYouSureData : public FragmentData {
  std::unique_ptr<QVBoxLayout> outerVbox;
  std::unique_ptr<QVBoxLayout> vbox;
  std::unique_ptr<QHBoxLayout> hbox;
  std::unique_ptr<QPushButton> wgtYes;
  std::unique_ptr<QPushButton> wgtNo;
  std::unique_ptr<QLabel> wgtPrompt;
  std::unique_ptr<QLabel> wgtIcon;
  std::unique_ptr<EvasiveButton> wgtFinalYes;
};

class FAreYouSure : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FAreYouSure(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  protected:
    void mouseMoveEvent(QMouseEvent* event) override;

  private slots:
    void onYesClick();
    void onNoClick();

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

    FAreYouSureData m_data;

    struct {
      QLayout* layout;
    } m_origParentData;

    TemplateMap m_templates;
    int m_count;
};


#endif
