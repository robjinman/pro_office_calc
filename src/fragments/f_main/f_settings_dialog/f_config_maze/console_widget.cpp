#include "fragments/f_main/f_settings_dialog/f_config_maze/console_widget.hpp"
#include "utils.hpp"


using std::string;


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

  m_commandPos = textCursor().position();
}

//===========================================
// ConsoleWidget::executeCommand
//===========================================
void ConsoleWidget::executeCommand(const string& cmd) {
  DBG_PRINT("Excecuting command \"" << cmd << "\"\n");
}

//===========================================
// ConsoleWidget::applyCommand
//===========================================
void ConsoleWidget::applyCommand() {
  QTextCursor cursor = textCursor();
  cursor.movePosition(QTextCursor::End);
  setTextCursor(cursor);

  insertPlainText("\n> ");
  m_commandPos = textCursor().position();

  executeCommand(m_buffer.toPlainText().toStdString());

  m_buffer.clear();
}

//===========================================
// ConsoleWidget::resetCursorPos
//===========================================
void ConsoleWidget::resetCursorPos() {
  QTextCursor cursor = textCursor();
  cursor.setPosition(m_commandPos + m_buffer.textCursor().position());
  setTextCursor(cursor);
}

//===========================================
// ConsoleWidget::syncCommandText
//===========================================
void ConsoleWidget::syncCommandText() {
  QTextCursor cursor = textCursor();
  cursor.setPosition(m_commandPos);

  while (!cursor.atEnd()) {
    cursor.deleteChar();
  }

  QString str = m_buffer.toPlainText();
  insertPlainText(str);
}

//===========================================
// ConsoleWidget::keyPressEvent
//===========================================
void ConsoleWidget::keyPressEvent(QKeyEvent* event) {
  resetCursorPos();

  switch (event->key()) {
    case Qt::Key_Return:
      applyCommand();
      break;
    default: {
      m_buffer.keyPressEvent(event);
      syncCommandText();
    }
  }

  resetCursorPos();
}
