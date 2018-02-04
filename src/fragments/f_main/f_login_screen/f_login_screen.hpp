#ifndef __PROCALC_FRAGMENTS_F_LOGIN_SCREEN_HPP__
#define __PROCALC_FRAGMENTS_F_LOGIN_SCREEN_HPP__


#include <memory>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QLineEdit>
#include "fragment.hpp"
#include "event.hpp"


class EventSystem;

struct FLoginScreenData : public FragmentData {
  std::unique_ptr<QLineEdit> wgtUser;
  std::unique_ptr<QLineEdit> wgtPassword;
  std::string password;
};

struct PasswordGeneratedEvent : public Event {
  PasswordGeneratedEvent(const std::string& password)
    : Event("PasswordGeneratedEvent"),
      password(password) {}

  std::string password;
};

class FLoginScreen : public QLabel, public Fragment {
  Q_OBJECT

  public:
    FLoginScreen(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FLoginScreen() override;

  private slots:
    void onLoginAttempt();

  private:
    FLoginScreenData m_data;

    struct {
      QWidget* centralWidget;
    } m_origParentState;

    int m_pwdGenEventId;
};


#endif
