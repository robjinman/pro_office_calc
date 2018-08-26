#include <cassert>
#include "fragment.hpp"
#include "fragment_spec.hpp"
#include "fragment_factory.hpp"
#include "utils.hpp"


using std::string;


//===========================================
// FragmentData::~FragmentData
//===========================================
FragmentData::~FragmentData() {}

//===========================================
// Fragment::Fragment
//===========================================
Fragment::Fragment(const string& name, Fragment& parent, FragmentData& parentData,
  FragmentData& ownData, const CommonFragData& commonData)
  : commonData(commonData),
    m_name(name),
    m_parent(&parent),
    m_parentData(&parentData),
    m_ownData(ownData) {}

//===========================================
// Fragment::Fragment
//===========================================
Fragment::Fragment(const string& name, FragmentData& ownData, const CommonFragData& commonData)
  : commonData(commonData),
    m_name(name),
    m_parent(nullptr),
    m_parentData(nullptr),
    m_ownData(ownData) {}

//===========================================
// Fragment::name
//===========================================
const string& Fragment::name() const {
  return m_name;
}

//===========================================
// Fragment::removeChildren
//
// Remove unused child fragments, recursively triggering their cleanUp methods
//===========================================
void Fragment::removeChildren(const FragmentSpec& spec, bool hardReset) {
  for (auto it = spec.specs().begin(); it != spec.specs().end(); ++it) {
    const string& chName = it->first;
    const FragmentSpec& chSpec = *it->second;

    if (!chSpec.isEnabled() || hardReset) {
      auto jt = m_children.find(chName);
      if (jt != m_children.end()) {
        Fragment& chFrag = *jt->second;

        chFrag.detach();
        m_children.erase(jt);
      }
    }
  }
}

//===========================================
// Fragment::constructNewChildren
//
// Construct any newly enabled child fragments
//===========================================
void Fragment::constructNewChildren(const FragmentSpec& spec) {
  for (auto it = spec.specs().begin(); it != spec.specs().end(); ++it) {
    const string& chName = it->first;
    const FragmentSpec& chSpec = *it->second;

    if (chSpec.isEnabled()) {
      if (m_children.find(chName) == m_children.end()) {
        Fragment* frag = constructFragment(chSpec.type(), *this, m_ownData, commonData);
        m_children.insert(std::make_pair(chName, pFragment_t(frag)));
      }
    }
  }
}

//===========================================
// Fragment::rebuildChildren
//===========================================
void Fragment::rebuildChildren(const FragmentSpec& spec, bool hardReset) {
  for (auto it = m_children.begin(); it != m_children.end(); ++it) {
    const string& name = it->first;
    Fragment& frag = *it->second;

    frag.rebuild(spec.spec(name), hardReset);
  }
}

//===========================================
// Fragment::rebuild
//===========================================
void Fragment::rebuild(const FragmentSpec& spec, bool hardReset) {
  removeChildren(spec, hardReset);
  reload(spec);
  constructNewChildren(spec);
  rebuildChildren(spec, hardReset);
}

//===========================================
// Fragment::detach
//===========================================
void Fragment::detach() {
  for (auto it = m_children.begin(); it != m_children.end(); ++it) {
    Fragment& frag = *it->second;
    frag.detach();
  }

  cleanUp();
}

//===========================================
// Fragment::~Fragment
//===========================================
Fragment::~Fragment() {}
