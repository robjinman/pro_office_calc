#ifndef __PROCALC_CONSOLE_WIDGET_HPP__
#define __PROCALC_CONSOLE_WIDGET_HPP__


#include <string>
#include <map>
#include <vector>
#include <functional>
#include <deque>
#include <QPlainTextEdit>


class AppConfig;

class ConsoleWidget : public QPlainTextEdit {
  Q_OBJECT

  public:
    typedef std::vector<std::string> ArgList;
    typedef std::function<std::string(const ArgList&)> CommandFn;

    ConsoleWidget(const AppConfig& appConfig, const std::string& initialContent,
      std::vector<std::string> initialHistory = {});

    void addCommand(const std::string& name, const CommandFn& fn);

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
    std::map<std::string, CommandFn> m_commandFns;
};


#endif
