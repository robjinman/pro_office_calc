#ifndef __PROCALC_FRAGMENTS_F_CONFIG_MAZE_CONSOLE_WIDGET_HPP__
#define __PROCALC_FRAGMENTS_F_CONFIG_MAZE_CONSOLE_WIDGET_HPP__


#include <string>
#include <map>
#include <deque>
#include <QPlainTextEdit>


class ConsoleWidget : public QPlainTextEdit {
  Q_OBJECT

  public:
    ConsoleWidget();

  protected:
    void keyPressEvent(QKeyEvent* event) override;

  private:
    struct OffscreenTextEdit : public QPlainTextEdit {
      void keyPressEvent(QKeyEvent* event) {
        QPlainTextEdit::keyPressEvent(event);
      }
    } m_buffer;

    void applyCommand();
    std::string executeCommand(const std::string& cmd);
    void resetCursorPos();
    void cursorToEnd();
    void syncCommandText();

    int m_commandPos;
    std::deque<std::string> m_commandHistory;
    int m_historyIdx = -1;
    std::map<std::string, std::string> m_knownCommands;
};


#endif
