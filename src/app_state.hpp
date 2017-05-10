#ifndef __PROCALC_APP_STATE_HPP__
#define __PROCALC_APP_STATE_HPP__


#include <type_traits>
#include <istream>
#include <ostream>
#include <memory>


#define SERIALIZE(x) \
  virtual void serialize(std::ostream& os) const override { \
    os << x; \
  }

#define DESERIALIZE(x) \
  virtual void deserialize(std::istream& is) override { \
    is >> x; \
  }


struct AppState {
  virtual void serialize(std::ostream& os) const = 0;
  virtual void deserialize(std::istream& is) = 0;

  virtual ~AppState() {}
};

typedef std::unique_ptr<AppState> pAppState_t;

std::ostream& operator<<(std::ostream& os, const AppState& state);
std::istream& operator>>(std::istream& is, AppState& state);

struct PreferencesSub1State : public AppState {
  int something1;
  float something2;
  std::string foo;

  SERIALIZE(something1 << something2 << foo)
  DESERIALIZE(foo >> something2 >> something1)
};

struct PreferencesSub2State : public AppState {
  long somethingElse1;
  double somethingDifferent2;

  SERIALIZE(somethingElse1 << somethingDifferent2)
  DESERIALIZE(somethingDifferent2 >> somethingElse1)
};

struct PreferencesState : public AppState {
  int fieldA;
  int fieldB;

  pAppState_t subState = pAppState_t(new PreferencesSub1State);

  SERIALIZE(fieldA << fieldB << *subState)
  DESERIALIZE(*subState >> fieldB >> fieldA)
};

struct MainSub0State : public AppState {
  int count = 10;

  SERIALIZE(count)
  DESERIALIZE(count)
};

struct MainSub1State : public AppState {
  bool divByZero = false;

  SERIALIZE(divByZero)
  DESERIALIZE(divByZero)
};

struct MainSub2State : public AppState {
  int a;

  SERIALIZE(a)
  DESERIALIZE(a)
};

struct MainState : public AppState {
  int level = 0;
  pAppState_t subState = pAppState_t(new MainSub0State);

  pAppState_t preferencesState = pAppState_t(new PreferencesState);

  SERIALIZE(level << *subState << *preferencesState)
  DESERIALIZE(*preferencesState >> *subState >> level)
};

/*
class AppConfig;

struct AppState {
  int level = 0;

  void load(const AppConfig& conf);
  void persist(const AppConfig& conf) const;
};*/


#endif
