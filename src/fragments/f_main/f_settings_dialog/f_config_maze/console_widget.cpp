#include "fragments/f_main/f_settings_dialog/f_config_maze/console_widget.hpp"


//===========================================
// ConsoleWidget::ConsoleWidget
//===========================================
ConsoleWidget::ConsoleWidget()
  : QPlainTextEdit(nullptr) {

  document()->setMaximumBlockCount(80);

  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::black);
  p.setColor(QPalette::Text, Qt::white);
  setPalette(p);
}
