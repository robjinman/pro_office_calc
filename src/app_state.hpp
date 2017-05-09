#ifndef __PROCALC_APP_STATE_HPP__
#define __PROCALC_APP_STATE_HPP__


#include <type_traits>
#include <istream>
#include <ostream>
#include <memory>


struct AppStateNew {
  virtual void serialize(std::ostream& os) const = 0;
  virtual void deserialize(std::istream& is) = 0;

  virtual ~AppStateNew() {}
};

typedef std::unique_ptr<AppStateNew> pAppStateNew_t;

std::ostream& operator<<(std::ostream& os, const AppStateNew& state);
std::istream& operator>>(std::istream& is, AppStateNew& state);

struct PreferencesSub1State : public AppStateNew {
  int something1;
  float something2;
  std::string foo;

  virtual void serialize(std::ostream& os) const override {
    os << something1 << something2 << foo;
  }

  virtual void deserialize(std::istream& is) override {
    is >> foo >> something2 >> something1;
  }
};

struct PreferencesSub2State : public AppStateNew {
  long somethingElse1;
  double somethingDifferent2;

  virtual void serialize(std::ostream& os) const override {
    os << somethingElse1 << somethingDifferent2;
  }

  virtual void deserialize(std::istream& is) override {
    is >> somethingDifferent2 >> somethingElse1;
  }
};

struct PreferencesState : public AppStateNew {
  int fieldA;
  int fieldB;

  pAppStateNew_t subState = pAppStateNew_t(new PreferencesSub1State);

  virtual void serialize(std::ostream& os) const override {
    os << fieldA << fieldB;
  }

  virtual void deserialize(std::istream& is) override {
    is >> fieldB >> fieldA;
  }
};

struct MainSub1State : public AppStateNew {
  bool divByZero = false;

  virtual void serialize(std::ostream& os) const override {
    os << divByZero;
  }

  virtual void deserialize(std::istream& is) override {
    is >> divByZero;
  }
};

struct MainState : public AppStateNew {
  int appClosedCount = 0;
  pAppStateNew_t subState = pAppStateNew_t(new MainSub1State);

  pAppStateNew_t preferencesState = pAppStateNew_t(new PreferencesState);

  virtual void serialize(std::ostream& os) const override {
    os << appClosedCount << *subState << *preferencesState;
  }

  virtual void deserialize(std::istream& is) override {
    is >> *preferencesState >> *subState >> appClosedCount;
  }
};


class AppConfig;

struct AppState {
  int level = 0;

  void load(const AppConfig& conf);
  void persist(const AppConfig& conf) const;
};


#endif
