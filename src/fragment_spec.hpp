#ifndef __PROCALC_FRAGMENT_SPEC_HPP__
#define __PROCALC_FRAGMENT_SPEC_HPP__


#include <vector>
#include <map>
#include <string>


class FragmentSpec {
  public:
    FragmentSpec(const std::string& name, std::vector<const FragmentSpec*> specs);

    const std::map<std::string, const FragmentSpec*>& specs() const;
    const FragmentSpec& spec(const std::string& name) const;
    const std::string& name() const;
    void setEnabled(bool b);
    bool isEnabled() const;

    virtual ~FragmentSpec() = 0;

  private:
    void populateChildrenMap() const;

    bool m_enabled;
    std::string m_name;
    mutable std::vector<const FragmentSpec*> m_tmpChildren;
    mutable std::map<std::string, const FragmentSpec*> m_children;
};


#endif
