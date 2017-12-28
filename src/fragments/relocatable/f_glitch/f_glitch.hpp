#ifndef __PROCALC_FRAGMENTS_F_GLITCH_HPP__
#define __PROCALC_FRAGMENTS_F_GLITCH_HPP__


#include <random>
#include <QLabel>
#include <QImage>
#include <QTimer>
#include "fragment.hpp"


struct FGlitchData : public FragmentData {};

class FGlitch : public QLabel, public Fragment {
  Q_OBJECT

  public:
    FGlitch(Fragment& parent, FragmentData& parentData);

    virtual void initialise(const FragmentSpec& spec) override;
    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FGlitch() override;

  public slots:
    void tick();

  private:
    FGlitchData m_data;

    double m_glitchFreqMin;
    double m_glitchFreqMax;
    double m_glitchDuration;

    std::unique_ptr<QImage> m_glitchBuffer;
    std::unique_ptr<QTimer> m_glitchTimer;
    std::mt19937 m_randEngine;
};


#endif
