#include <regex>
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

  m_commandHistory.insert(m_commandHistory.begin(), {
    "logouut",
    "hardreset -u rob -p passw0rd"
  });

  m_knownCommands["logout"] = "Error";
  m_knownCommands["hardreset"] = "Error";

  m_commandPos = textCursor().position();
}

//===========================================
// ConsoleWidget::executeCommand
//===========================================
string ConsoleWidget::executeCommand(const string& commandString) {
  std::regex rx("^([^\\s]+).*$");
  std::smatch m;

  string output;

  if (std::regex_match(commandString, m, rx)) {
    m_commandHistory.push_front(commandString);

    string cmd = m.str(1);

    auto it = m_knownCommands.find(cmd);
    if (it != m_knownCommands.end()) {
      output = it->second;
    }
    else {
      output = "Unknown command";
    }
  }

  return output;
}

//===========================================
// ConsoleWidget::cursorToEnd
//===========================================
void ConsoleWidget::cursorToEnd() {
  QTextCursor cursor = textCursor();
  cursor.movePosition(QTextCursor::End);
  setTextCursor(cursor);
}

//===========================================
// ConsoleWidget::applyCommand
//===========================================
void ConsoleWidget::applyCommand() {
  cursorToEnd();

  string output = executeCommand(m_buffer.toPlainText().toStdString());

  insertPlainText("\n");
  insertPlainText(output.c_str());
  insertPlainText("\n> ");

  m_commandPos = textCursor().position();

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
      m_historyIdx = -1;
      break;
    case Qt::Key_Up:
      if (m_historyIdx < static_cast<int>(m_commandHistory.size()) - 1) {
        m_buffer.clear();
        m_buffer.insertPlainText(m_commandHistory[++m_historyIdx].c_str());
        syncCommandText();
      }
      break;
    case Qt::Key_Down:
      if (m_historyIdx > 0) {
        m_buffer.clear();
        m_buffer.insertPlainText(m_commandHistory[--m_historyIdx].c_str());
      }
      else {
        m_historyIdx = -1;
        m_buffer.clear();
      }
      syncCommandText();
      break;
    default: {
      m_buffer.keyPressEvent(event);
      m_historyIdx = -1;
      syncCommandText();
    }
  }

  resetCursorPos();
}
