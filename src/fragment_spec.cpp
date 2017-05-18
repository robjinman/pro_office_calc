#include "fragment_spec.hpp"


//===========================================
// FragmentSpec::FragmentSpec
//===========================================
FragmentSpec::FragmentSpec(const std::string& name, std::vector<const FragmentSpec*> specs)
  : m_enabled(false),
    m_name(name),
    m_tmpChildren(specs) {}

//===========================================
// FragmentSpec::setEnabled
//===========================================
void FragmentSpec::setEnabled(bool b) {
  m_enabled = b;
}

//===========================================
// FragmentSpec::isEnabled
//===========================================
bool FragmentSpec::isEnabled() const {
  return m_enabled;
}

//===========================================
// FragmentSpec::specs
//===========================================
const std::map<std::string, const FragmentSpec*>& FragmentSpec::specs() const {
  populateChildrenMap();
  return m_children;
}

//===========================================
// FragmentSpec::populateChildrenMap
//===========================================
void FragmentSpec::populateChildrenMap() const {
  if (m_tmpChildren.size() > 0) {
    for (auto it = m_tmpChildren.begin(); it != m_tmpChildren.end(); ++it) {
      m_children.insert({(*it)->name(), *it});
    }
    m_tmpChildren.clear();
  }
}

//===========================================
// FragmentSpec::spec
//===========================================
const FragmentSpec& FragmentSpec::spec(const std::string& name) const {
  populateChildrenMap();
  return *m_children.at(name);
}

//===========================================
// FragmentSpec::name
//===========================================
const std::string& FragmentSpec::name() const {
  return m_name;
}

//===========================================
// FragmentSpec::~FragmentSpec
//===========================================
FragmentSpec::~FragmentSpec() {}
