#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_text_editor/f_text_editor.hpp"
#include "fragments/f_main/f_app_dialog/f_text_editor/f_text_editor_spec.hpp"
#include "utils.hpp"


//===========================================
// FTextEditor::FTextEditor
//===========================================
FTextEditor::FTextEditor(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FTextEditor", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FTextEditor::FTextEditor\n");
}

//===========================================
// FTextEditor::reload
//===========================================
void FTextEditor::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FTextEditor::reload\n");

  const auto& spec = dynamic_cast<const FTextEditorSpec&>(spec_);
  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  m_data.vbox = makeQtObjPtr<QVBoxLayout>();
  setLayout(m_data.vbox.get());

  m_data.wgtTextBrowser = makeQtObjPtr<QTextBrowser>(this);
  m_data.vbox->addWidget(m_data.wgtTextBrowser.get());

  m_data.wgtTextBrowser->setText(spec.content);
}


//===========================================
// FTextEditor::cleanUp
//===========================================
void FTextEditor::cleanUp() {
  DBG_PRINT("FTextEditor::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);
}

//===========================================
// FTextEditor::~FTextEditor
//===========================================
FTextEditor::~FTextEditor() {
  DBG_PRINT("FTextEditor::~FTextEditor\n");
}
