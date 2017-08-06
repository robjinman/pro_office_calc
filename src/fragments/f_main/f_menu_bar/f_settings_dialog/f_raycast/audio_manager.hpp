#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_AUDIO_MANAGER_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_AUDIO_MANAGER_HPP__


#include <string>
#include <memory>
#include <QSoundEffect>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"


class AudioManager {
  public:
    void addSound(const std::string& name, const std::string& resourcePath);
    void addMusicTrack(const std::string& name, const std::string& resourcePath);

    void playSound(const std::string& name);
    void playSoundAtPos(const std::string& name, const Point& pos);

    void playMusic(const std::string& name);
    void stopMusic();
    void setMusicVolume(double volume);

  private:
    std::map<std::string, std::unique_ptr<QSoundEffect>> m_sounds;
};


#endif
