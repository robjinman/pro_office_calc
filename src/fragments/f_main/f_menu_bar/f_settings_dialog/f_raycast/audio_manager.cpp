#include <QFileInfo>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/audio_manager.hpp"


using std::make_pair;
using std::string;
using std::unique_ptr;


//===========================================
// AudioManager::addSound
//===========================================
void AudioManager::addSound(const string& name, const string& resourcePath) {
  unique_ptr<QSoundEffect> sound(new QSoundEffect);

  QString absPath = QFileInfo(resourcePath.c_str()).absoluteFilePath();
  sound->setSource(QUrl::fromLocalFile(absPath));

  m_sounds.insert(make_pair(name, std::move(sound)));
}

//===========================================
// AudioManager::addMusicTrack
//===========================================
void AudioManager::addMusicTrack(const string& name, const string& resourcePath) {

}

//===========================================
// AudioManager::playSound
//===========================================
void AudioManager::playSound(const string& name) {
  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    it->second->play();
  }
}

//===========================================
// AudioManager::playSoundAtPos
//===========================================
void AudioManager::playSoundAtPos(const string& name, const Point& pos) {
  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) { // TODO
    it->second->play();
  }
}

//===========================================
// AudioManager::playMusic
//===========================================
void AudioManager::playMusic(const string& name) {

}

//===========================================
// AudioManager::stopMusic
//===========================================
void AudioManager::stopMusic() {

}

//===========================================
// AudioManager::setMusicVolume
//===========================================
void AudioManager::setMusicVolume(double volume) {

}
