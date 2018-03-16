#ifndef __PROCALC_FRAGMENTS_F_TEXT_EDITOR_HPP__
#define __PROCALC_FRAGMENTS_F_TEXT_EDITOR_HPP__


#include <QVBoxLayout>
#include <QTextBrowser>
#include <QMargins>
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"


struct FTextEditorData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<QTextBrowser> wgtTextBrowser;
};

class FTextEditor : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FTextEditor(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FTextEditor() override;

  private:
    FTextEditorData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
