#include <cassert>
#include "fragment.hpp"
#include "fragment_spec.hpp"
#include "fragment_factory.hpp"


//===========================================
// FragmentData::~FragmentData
//===========================================
FragmentData::~FragmentData() {}

//===========================================
// Fragment::Fragment
//===========================================
Fragment::Fragment(const std::string& name, Fragment& parent,
  FragmentData& parentData, FragmentData& ownData)
  : m_name(name),
    m_parent(&parent),
    m_parentData(&parentData),
    m_ownData(ownData) {}

//===========================================
// Fragment::Fragment
//===========================================
Fragment::Fragment(const std::string& name, FragmentData& ownData)
  : m_name(name),
    m_parent(nullptr),
    m_parentData(nullptr),
    m_ownData(ownData) {}

//===========================================
// Fragment::name
//===========================================
const std::string& Fragment::name() const {
  return m_name;
}

//===========================================
// Fragment::rebuild
//===========================================
void Fragment::rebuild(const FragmentSpec& spec) {
  for (auto it = spec.specs().begin(); it != spec.specs().end(); ++it) {
    if (it->second->isEnabled()) {
      if (m_children.find(it->first) == m_children.end()) {
        Fragment* frag = constructFragment(it->first, *this, m_ownData);
        m_children.insert(std::make_pair(it->first, pFragment_t(frag)));
      }
    }
    else {
      auto ch = m_children.find(it->first);
      if (ch != m_children.end()) {
        ch->second->cleanUp();
        m_children.erase(ch);
      }
    }
  }

  for (auto it = m_children.begin(); it != m_children.end(); ++it) {
    it->second->rebuild(spec.spec(it->first));
  }
}

//===========================================
// Fragment::~Fragment
//===========================================
Fragment::~Fragment() {}
