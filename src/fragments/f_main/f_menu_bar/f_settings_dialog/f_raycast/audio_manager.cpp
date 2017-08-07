#include <QFileInfo>
#include <QUrl>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/audio_manager.hpp"


using std::make_pair;
using std::string;
using std::unique_ptr;


//===========================================
// AudioManager::AudioManager
//===========================================
AudioManager::AudioManager() {
  m_musicVolume = 0.5;
  setMasterVolume(0.5);

  m_mediaPlayer.setPlaylist(&m_playlist);
}

//===========================================
// AudioManager::addSound
//===========================================
void AudioManager::addSound(const string& name, const string& resourcePath) {
  pSoundEffect_t sound(new SoundEffect);

  QString absPath = QFileInfo(resourcePath.c_str()).absoluteFilePath();
  sound->sound.setSource(QUrl::fromLocalFile(absPath));

  m_sounds.insert(make_pair(name, std::move(sound)));
}

//===========================================
// AudioManager::addMusicTrack
//===========================================
void AudioManager::addMusicTrack(const string& name, const string& resourcePath) {
  QString absPath = QFileInfo(resourcePath.c_str()).absoluteFilePath();
  m_musicTracks[name] = QMediaContent(QUrl::fromLocalFile(absPath));
}

//===========================================
// AudioManager::playSound
//===========================================
void AudioManager::playSound(const string& name) {
  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    sound.sound.setVolume(m_masterVolume * sound.volume);
    sound.sound.play();
  }
}

//===========================================
// AudioManager::playSoundAtPos
//===========================================
void AudioManager::playSoundAtPos(const string& name, const Point& pos) {
  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) { // TODO
    SoundEffect& sound = *it->second;

    sound.sound.setVolume(m_masterVolume * sound.volume);
    sound.sound.play();
  }
}

//===========================================
// AudioManager::playMusic
//===========================================
void AudioManager::playMusic(const string& name) {
  auto it = m_musicTracks.find(name);

  if (it != m_musicTracks.end()) {
    m_playlist.clear();
    m_playlist.addMedia(it->second);
    m_playlist.setPlaybackMode(QMediaPlaylist::Loop);

    m_mediaPlayer.play();
  }
}

//===========================================
// AudioManager::stopMusic
//===========================================
void AudioManager::stopMusic() {
  m_mediaPlayer.stop();
}

//===========================================
// AudioManager::setMusicVolume
//===========================================
void AudioManager::setMusicVolume(double volume) {
  m_musicVolume = volume;
  m_mediaPlayer.setVolume(m_masterVolume * m_musicVolume * 100);
}

//===========================================
// AudioManager::setMasterVolume
//===========================================
void AudioManager::setMasterVolume(double volume) {
  m_masterVolume = volume;
  setMusicVolume(m_musicVolume);
}
