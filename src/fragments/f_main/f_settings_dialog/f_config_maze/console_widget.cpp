#include "fragments/f_main/f_settings_dialog/f_config_maze/console_widget.hpp"


//===========================================
// ConsoleWidget::ConsoleWidget
//===========================================
ConsoleWidget::ConsoleWidget()
  : QPlainTextEdit(nullptr) {

  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::black);
  p.setColor(QPalette::Text, Qt::white);
  setPalette(p);

  QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  document()->setDefaultFont(font);

  setReadOnly(true);

  insertPlainText(
    "┌───────────────────────────────────────┐\n"
    "│ Admin Console v1.1.16                 │\n"
    "├───────────────────────────────────────┤\n"
    "│ Date and time    1993/03/14 16:11:23  │\n"
    "│ Logged in as     rob                  │\n"
    "│ Logged in since  1993/03/14 15:22:49  │\n"
    "│                                       │\n"
    "│ ↑↓ cycle history                      │\n"
    "└───────────────────────────────────────┘\n"
    "> ");
}
