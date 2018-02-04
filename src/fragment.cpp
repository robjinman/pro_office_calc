#include <cassert>
#include "fragment.hpp"
#include "fragment_spec.hpp"
#include "fragment_factory.hpp"


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
// Fragment::rebuild
//===========================================
void Fragment::rebuild(const FragmentSpec& spec) {
  // Remove unused child fragments, recursively triggering their cleanUp methods
  //
  for (auto it = spec.specs().begin(); it != spec.specs().end(); ++it) {
    const string& chName = it->first;
    const FragmentSpec& chSpec = *it->second;

    if (!chSpec.isEnabled()) {
      auto jt = m_children.find(chName);
      if (jt != m_children.end()) {
        Fragment& chFrag = *jt->second;

        chFrag.detach();
        m_children.erase(jt);
      }
    }
  }

  // Call reload before constructing children
  //
  reload(spec);

  // Construct any newly enabled child fragments
  //
  for (auto it = spec.specs().begin(); it != spec.specs().end(); ++it) {
    const string& chName = it->first;
    const FragmentSpec& chSpec = *it->second;

    if (chSpec.isEnabled()) {
      if (m_children.find(chName) == m_children.end()) {
        Fragment* frag = constructFragment(chName, *this, m_ownData, commonData);
        m_children.insert(std::make_pair(chName, pFragment_t(frag)));
      }
    }
  }

  // Rebuild children
  //
  for (auto it = m_children.begin(); it != m_children.end(); ++it) {
    const string& name = it->first;
    Fragment& frag = *it->second;

    frag.rebuild(spec.spec(name));
  }
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
