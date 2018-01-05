#ifndef __PROCALC_FRAGMENTS_F_TETROMINOS_HPP__
#define __PROCALC_FRAGMENTS_F_TETROMINOS_HPP__


#include <memory>
#include <QLabel>
#include "fragment.hpp"
#include "event.hpp"


class EventSystem;

struct FTetrominosData : public FragmentData {
  EventSystem* eventSystem;
};

class FTetrominos : public QLabel, public Fragment {
  Q_OBJECT

  public:
    FTetrominos(Fragment& parent, FragmentData& parentData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FTetrominos() override;

  private:
    FTetrominosData m_data;
};


#endif
