#ifndef __PROCALC_FRAGMENTS_F_MAIL_CLIENT_HPP__
#define __PROCALC_FRAGMENTS_F_MAIL_CLIENT_HPP__


#include <memory>
#include <QDialog>
#include "fragment.hpp"


struct FMailClientData : public FragmentData {

};

class FMailClient : public QDialog, public Fragment {
  Q_OBJECT

  public:
    FMailClient(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMailClient() override;

  private:
    FMailClientData m_data;

    int m_eventIdx;
};


#endif
