#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"


entityId_t Component::nextId = 0;


//===========================================
// Component::getNextId
//===========================================
entityId_t Component::getNextId() {
  return nextId++;
}
