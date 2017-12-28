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
    Fragment(const std::string& name, FragmentData& ownData);

    Fragment(const std::string& name, Fragment& parent,
      FragmentData& parentData, FragmentData& ownData);

    const std::string& name() const;

    // Rebuild fragment tree by adding/removing children and calling their respective
    // lifecycle functions
    void rebuild(const FragmentSpec& spec);

    // Construct and attach to / modify parent. Called immediately after construction
    virtual void initialise(const FragmentSpec& spec) = 0;

    // Reset fragment's state on rebuild. Called when the app state changes and the
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
