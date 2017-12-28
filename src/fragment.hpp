#ifndef __PROCALC_FRAGMENT_HPP__
#define __PROCALC_FRAGMENT_HPP__


#include <string>
#include <map>
#include <memory>


struct FragmentData {
  virtual ~FragmentData() = 0;
};


class FragmentSpec;
class Fragment;

typedef std::unique_ptr<Fragment> pFragment_t;

class Fragment {
  public:
    // Use constructor to initialise the fragment and attach to / modify the parent

    Fragment(const std::string& name, FragmentData& ownData);

    Fragment(const std::string& name, Fragment& parent,
      FragmentData& parentData, FragmentData& ownData);

    // Rebuild fragment tree by adding/removing children and calling their respective
    // lifecycle functions
    void rebuild(const FragmentSpec& spec);

    // Re-initialise fragment with new spec. Called when the app state changes and the
    // fragment tree is rebuilt
    virtual void reload(const FragmentSpec& spec) = 0;

    // Detach from and reverse modifications to parent. Called prior to being removed
    // from the fragment tree and destroyed
    virtual void cleanUp() = 0;

    template<class T>
    T& parentFragData() {
      return dynamic_cast<T&>(*m_parentData);
    }

    template<class T>
    T& parentFrag() {
      return dynamic_cast<T&>(*m_parent);
    }

    const std::string& name() const;

    virtual ~Fragment() = 0;

  private:
    std::string m_name;
    Fragment* m_parent;
    FragmentData* m_parentData;
    FragmentData& m_ownData;

    std::map<std::string, pFragment_t> m_children;

    void detach();
};


#endif
