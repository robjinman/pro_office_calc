#ifndef __PROCALC_RAYCAST_AUDIO_MANAGER_HPP__
#define __PROCALC_RAYCAST_AUDIO_MANAGER_HPP__


#include <string>
#include <memory>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMediaContent>
#include "raycast/geometry.hpp"


struct SoundEffect {
  QSoundEffect sound;
  double volume = 1.0;
};

typedef std::unique_ptr<SoundEffect> pSoundEffect_t;

class EntityManager;

class AudioService {
  public:
    AudioService(EntityManager& entityManager);

    void addSound(const std::string& name, const std::string& resourcePath);
    void addMusicTrack(const std::string& name, const std::string& resourcePath);

    void playSound(const std::string& name, bool loop = false);
    void playSoundAtPos(const std::string& name, const Point& pos, bool loop = false);
    void stopSound(const std::string& name);
    bool soundIsPlaying(const std::string& name) const;

    void playMusic(const std::string& name, bool loop = true);
    void stopMusic();
    void setMusicVolume(double volume);

    void setMasterVolume(double volume);

  private:
    EntityManager& m_entityManager;
    QMediaPlayer m_mediaPlayer;
    QMediaPlaylist m_playlist;
    std::map<std::string, pSoundEffect_t> m_sounds;
    std::map<std::string, QMediaContent> m_musicTracks;
    double m_masterVolume;
    double m_musicVolume;
};


#endif
