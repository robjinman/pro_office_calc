#ifndef __PROCALC_FRAGMENTS_F_CONSOLE_HPP__
#define __PROCALC_FRAGMENTS_F_CONSOLE_HPP__


#include <QVBoxLayout>
#include <QMargins>
#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"
#include "event_system.hpp"
#include "console_widget.hpp"


struct FConsoleData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<ConsoleWidget> wgtConsole;
};

class FConsole : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FConsole(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FConsole() override;

  private:
    void addCommands(const std::vector<std::vector<std::string>>& commands);

    FConsoleData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;

    int m_commandsEntered = 0;
    EventHandle m_hCommandsGenerated;
};


#endif
