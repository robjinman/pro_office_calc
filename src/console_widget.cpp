#include <sstream>
#include "console_widget.hpp"


using std::vector;
using std::string;
using std::stringstream;
using std::istream_iterator;


//===========================================
// ConsoleWidget::ConsoleWidget
//===========================================
ConsoleWidget::ConsoleWidget(const string& initialContent, vector<string> initialHistory)
  : QPlainTextEdit(nullptr),
    m_commandHistory(initialHistory.begin(), initialHistory.end()) {

  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::black);
  p.setColor(QPalette::Text, Qt::white);
  setPalette(p);

  QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  document()->setDefaultFont(font);

  insertPlainText(initialContent.c_str());

  m_commandPos = textCursor().position();
}

//===========================================
// ConsoleWidget::addCommand
//===========================================
void ConsoleWidget::addCommand(const string& name, const ConsoleWidget::CommandFn& fn) {
  m_commandFns[name] = fn;
}

//===========================================
// ConsoleWidget::executeCommand
//===========================================
string ConsoleWidget::executeCommand(const string& commandString) {
  string output;
  stringstream ss(commandString);
  ArgList vec{istream_iterator<string>(ss), istream_iterator<string>{}};

  if (vec.size() > 0) {
    m_commandHistory.push_front(commandString);

    const string& cmd = vec[0];
    ArgList args(++vec.begin(), vec.end());

    auto it = m_commandFns.find(cmd);
    if (it != m_commandFns.end()) {
      output = it->second(args);
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
